.initializeCdf <- function(con, nrows = 1, ncols = 1,
                          nunits = 1, nqcunits = 0,
                          refseq = "",
                          unitnames = rep("", nunits),
                          qcunitpositions = rep(1, nqcunits),
                          unitpositions = rep(2, nunits),
                          qcUnitLengths = rep(0, nqcunits),
                          unitLengths = rep(0, nunits),
                          ...) {
    if(length(unitpositions) != nunits)
        stop("unitpositions argument is not matching nunits")
    if(length(qcunitpositions) != nqcunits)
        stop("qcunitpositions argument is not matching nqcunits")
    if(length(refseq) != 1)
        stop("refseq argument is wrong")
    lrefseq <- nchar(refseq)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # CDF header
    #
    # 1 Magic number. Always set to 67.                           [integer]
    # 2 Version number.                                           [integer]
    # 3 The number of columns of cells on the array.       [unsigned short]
    # 4 The number of rows of cells on the array.          [unsigned short]
    # 5 The number of units in the array not including QC units. The term 
    #   unit is an internal term which means probe set.           [integer]
    # 6 The number of QC units.                                   [integer]
    # 7 The length of the resequencing reference sequence.        [integer]
    # 8 The resequencing reference sequence.                    [char[len]]
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ## Magic number and version number
    writeBin(object = as.integer(c(67, 1)),
             con = con, size = 4, endian = "little")
    ## Ncols, Nrows
    writeBin(object = as.integer(c(ncols, nrows)),
             con = con, size = 2, endian = "little")
    ## NumberUnits, NumberQCUnits
    writeBin(object = as.integer(c(nunits, nqcunits)),
             con = con, size = 4, endian = "little")
    ## Length of refseqsequence
    writeBin(object = as.integer(lrefseq),
             con = con, size = 4, endian = "little")
    ## Refseqsequece
    if(lrefseq > 0)
        writeChar(as.character(refseq),
                  con = con, eos = NULL)

    # Current offset
    offset <- 24 + lrefseq;

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Unit names
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Write to raw vector (2*10^6 units => 122Mb; should be ok for now)
    # Since we can't create strings with '\0':s, we use '\377' (=0xFF),
    # write to raw and then replace '\377' with '\0'. Thus, unit names with
    # '\377' are invalid, but this should not be a real problem.
    pads <- sapply(0:64, FUN=function(x) paste(rep("\377", x), collapse=""));
    unitnames <- paste(unitnames, pads[64-nchar(unitnames)], sep="");
    raw <- raw(64*length(unitnames));
    raw <- writeBin(con=raw, unitnames, size=1);
    raw[raw == as.raw(255)] <- as.raw(0);
    writeBin(con=con, raw);
    rm(raw);
#    writeChar(con=con, as.character(unitnames), nchars=rep(64, nunits), eos=NULL)

    bytesOfUnitNames <- 64 * nunits;
    offset <- offset + bytesOfUnitNames;

    bytesOfQcUnits <- 4 * nqcunits;
    offset <- offset + bytesOfQcUnits;

    bytesOfUnits <- 4 * nunits;
    offset <- offset + bytesOfUnits;

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # QC units file positions
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (nqcunits > 0) {
      csum <- cumsum(qcUnitLengths);
      nextOffset <- csum[nqcunits];
      starts <- c(0, csum[-nqcunits]);
      starts <- as.integer(offset + starts);
      writeBin(starts, con = con, size = 4, endian = "little")
    } else {
      nextOffset <- 0;
#      starts <- 0;
#      starts <- as.integer(offset + starts);
#      writeBin(starts, con = con, size = 4, endian = "little")
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Units file positions
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    offset <- offset + nextOffset;
    if (nunits > 0) {
      csum <- cumsum(unitLengths);
      nextOffset <- csum[nunits];
      starts <- c(0, csum[-nunits]);
      starts <- as.integer(offset + starts);
      writeBin(starts, con = con, size = 4, endian = "little");
    } else {
      nextOffset <- 0;
    }

    # Return
    ## positions return argument
    offset <- 24 + lrefseq;
    bytesOfUnitNames <- 64 * nunits;
    bytesOfQcUnits <- 4 * nqcunits;
    positions <- list(unitName = offset,
                      qcunitFP = offset + bytesOfUnitNames,
                      unitFP = offset + bytesOfUnitNames + bytesOfQcUnits)
    positions;
} # initializeCdf()


.writeCdfUnit <- function(unit, con, unitname = NULL, positions = NULL,
                      addName = TRUE, addPosition = TRUE) {

    ## 1. Write the name in the start of the CDF file
    if(addName) {
        # NOTE: It is very expensive to jump around in the file
        # (file cache can't be optimized etc).  Typically the unit
        # names are know from the beginning and can be written all
        # at once instead. /HB
        seek(con = con, where = positions$unitName,
             origin = "start")
        suppressWarnings({
          writeChar(as.character(unitname), nchars = 64,
                    con = con, eos = NULL)
        })
        positions$unitName <- positions$unitName + 64
    }

    ## 2. Jump to the end to start writing the unit
    if(addPosition) {
      seek(con = con, where = 0, origin = "end", rw = "write")
      endPosition <- seek(con = con, where = 0, origin = "end", rw = "write")
    }

    ## 3. Write the unit
    unitTypes <- c(expression=1, genotyping=2, tag=3, 
                                             resequencing=4, unknown=5);
    unittype <- unitTypes[unit$unittype];
    unitDirections <- c(nodirection=0, sense=1, antisense=2, unknown=3);
    unitdirection <- unitDirections[unit$unitdirection];

    unittype <- switch(unit$unittype,
                       expression = 1,
                       genotyping = 2,
                       tag = 3,
                       resequencing = 4,
                       unknown = 5)
    unitdirection <- switch(unit$unitdirection,
                            nodirection = 0,
                            sense = 1,
                            antisense = 2,
                            unknown = 3)

    unitInfo <- as.integer(c(unittype, unitdirection,
                             unit$natoms, length(unit$blocks),
                             unit$ncells, unit$unitnumber,
                             unit$ncellsperatom))

    # Number of bytes: 2+1+4*4+1=20 bytes    
    writeBin(unitInfo[1],
             con = con, size = 2, endian = "little")
    writeBin(unitInfo[2],
             con = con, size = 1, endian = "little")
    writeBin(unitInfo[3:6],
             con = con, size = 4, endian = "little")
    writeBin(unitInfo[7],
             con = con, size = 1, endian = "little")

    ## Writing each block in turn
    # Number of bytes: (18+64)*nbrOfBlocks + 14*totalNbrOfCells bytes
    blockDirections <- c(nodirection=0, sense=1, antisense=2, unknown=3);
    for(iblock in seq(along.with = unit$blocks)) {
        block <- unit$blocks[[iblock]]
        blockdirection <- blockDirections[block$blockdirection];
        blockdirection <- switch(block$blockdirection,
                                 nodirection = 0,
                                 sense = 1,
                                 antisense = 2,
                                 unknown = 3)
        blockInfo <- as.integer(c(block$natoms, length(block$x),
                                  block$ncellsperatom,
                                  blockdirection, min(block$atoms, 0)))
       # Number of bytes: 2*4+2*1+2*4=18 bytes
        writeBin(blockInfo[1:2],
                 con = con, size = 4, endian = "little")
        writeBin(blockInfo[3:4],
                 con = con, size = 1, endian = "little")
        writeBin(blockInfo[5:6],
                 con = con, size = 4, endian = "little")

        # Number of bytes: 64 bytes
        suppressWarnings({
          writeChar(as.character(names(unit$blocks)[iblock]),
                    con = con, nchars = 64, eos = NULL) 
        })

        ## Writing each cell in turn
#        cells <- matrix(as.integer(c(block$atom, block$x,
#                                     block$y, block$indexpos)),
#                        ncol = 4)
        cells <- matrix(as.integer(c(block$indexpos, block$x,
                                     block$y, block$atom)),
                        ncol = 4)

        # Number of bytes: 14*nbrOfCells bytes
        for(icell in seq(along.with = block$x)) {
            # Number of bytes: 1*4+2*2+1*4+1*2=14 bytes
            writeBin(cells[icell, 1],
                     con = con, size = 4, endian = "little")
            writeBin(cells[icell, 2:3],
                     con = con, size = 2, endian = "little")
            writeBin(cells[icell, 4],
                     con = con, size = 4, endian = "little")
            writeChar(as.character(c(block$pbase[icell],
                                     block$tbase[icell])),
                      con = con, nchars = c(1,1), eos = NULL)
        }
    }
    ## 4. Go back to the start of the file and write
    ##    where the unit starts
    if(addPosition) {
        # NOTE: It is very expensive to jump around in the file! 
        # It would be good if positions can be written all at
        # once instead, either by figuring it out from the beginning
        # or by updating at the end. /HB
        seek(con = con, where = positions$unitFP,
             origin = "start", rw = "write")
        writeBin(as.integer(endPosition),
                 con = con, size = 4, endian = "little")
        positions$unitFP <- positions$unitFP + 4
    }

    ## 5. Update the positions and return
    positions;
} # writeCdfUnit()


.writeCdfUnit2 <- function(unit, con, unitname = NULL, positions = NULL,
                      addName = TRUE, addPosition = TRUE) {
    ## 1. Write the name in the start of the CDF file
    if(addName) {
        seek(con = con, where = positions$unitName,
             origin = "start")
        suppressWarnings({
          writeChar(as.character(unitname), nchars = 64,
                    con = con, eos = NULL)
        })
        positions$unitName <- positions$unitName + 64
    }
    ## 2. Jump to the end to start writing the unit
    seek(con = con, where = 0, origin = "end", rw = "write")
    endPosition <- seek(con = con, where = 0,
                        origin = "end", rw = "write")
    ## 3. Write the unit
    unittype <- switch(unit$unittype,
                       expression = 1,
                       genotyping = 2,
                       tag = 3,
                       resequencing = 4,
                       unknown = 5)
    unitdirection <- switch(unit$unitdirection,
                            nodirection = 0,
                            sense = 1,
                            antisense = 2,
                            unknown = 3)
    unitInfo <- as.integer(c(unittype, unitdirection,
                             unit$natoms, length(unit$blocks),
                             unit$ncells, unit$unitnumber,
                             unit$ncellsperatom))
    rawUnit <- raw(...)
    raw[1] <- writeBin(unitInfo[1], con = raw(),
                       size = 2, endian = "little")
    raw[c(3,20)] <- writeBin(unitInfo[c(2,7)], con = raw(),
                             size = 1, endian = "little")
    raw[4:19] <- writeBin(unitInfo[3:6], con = raw(),
                          size = 4, endian = "little")
    ## Unitinfo = 20 bytes
    ## Blockinfo = 18 + 64 = 82 bytes
    ## Cellinfo = 14 bytes
    ## total = 20 + 82*nblock + 14*ncells
    offset <- 20
    ## Writing each block in turn
    for(iblock in seq(along.with = unit$blocks)) {
        block <- unit$blocks[[iblock]]
        blockdirection <- switch(block$blockdirection,
                                 nodirection = 0,
                                 sense = 1,
                                 antisense = 2,
                                 unknown = 3)
        blockInfo <- as.integer(c(block$natoms, length(block$x),
                                  block$ncellsperatom,
                                  blockdirection, min(block$atoms, 0)))
        raw[offset + c(1:8,11:18)] <- writeBin(blockInfo[c(1:2, 5:6)],
                                               con = raw(),
                                               size = 4, endian = "little")
        raw[offset + 9:10] <- writeBin(blockInfo[3:4], con = raw(),
                                       size = 1, endian = "little")
        raw[offset + 19:82] <- writeChar(as.character(names(unit$blocks)
                                                      [iblock]),
                                         con = raw(), nchars = 64, eos = NULL)
        offset <- offset + 82
        ## Writing each cell in turn
        cells <- matrix(as.integer(c(block$atom, block$x,
                                     block$y, block$indexpos)),
                        ncol = 4)


        
        for(icell in seq(along.with = block$x)) {
            writeBin(cells[icell, 1],
                     con = con, size = 4, endian = "little")
            writeBin(cells[icell, 2:3],
                     con = con, size = 2, endian = "little")
            writeBin(cells[icell, 4],
                     con = con, size = 4, endian = "little")
            writeChar(as.character(c(block$pbase[icell],
                                     block$tbase[icell])),
                      con = con, nchars = c(1,1), eos = NULL)
        }
    }
    ## 4. Go back to the start of the file and write
    ##    where the unit starts
    if(addPosition) {
        seek(con = con, where = positions$unitFP,
             origin = "start", rw = "write")
        writeBin(as.integer(endPosition),
                 con = con, size = 4, endian = "little")
        positions$unitFP <- positions$unitFP + 4
    }
    ## 5. Update the positions and return
    if(!is.null(positions))
        return(positions)
    else
        return(NULL)
}


.writeCdfQcUnit <- function(qcunit, con, positions = NULL,
                        addPosition = TRUE) {
    ## 1. Jump to end to write a qcunit
    if(addPosition) {
      seek(con = con, where = 0, origin = "end", rw = "write")
      endPosition <- seek(con = con, where = 0,
                        origin = "end", rw = "write")
    }

    ## 2. Actually write the qcunit
    type <- switch(qcunit$type,
                   unknown = 0,
                   checkerboardNegative = 1,
                   checkerboardPositive = 2,
                   hybeNegative = 3,
                   hybePositive = 4,
                   textFeaturesNegative = 5,
                   textFeaturesPositive = 6,
                   centralNegative = 7,
                   centralPositive = 8,
                   geneExpNegative = 9,
                   geneExpPositive = 10,
                   cycleFidelityNegative = 11,
                   cycleFidelityPositive = 12,
                   centralCrossNegative = 13,
                   centralCrossPositive = 14,
                   crossHybeNegative = 15,
                   crossHybePositive = 16,
                   SpatialNormNegative = 17,
                   SpatialNormPositive = 18)

    # Write 2 + 4 bytes
    nbrOfBytes <- 6;
    qcunitInfo <- as.integer(c(type, qcunit$ncells))
    writeBin(qcunitInfo[1], con = con, size = 2, endian = "little")
    writeBin(qcunitInfo[2], con = con, size = 4, endian = "little")

    # Write 2 + 4 bytes
    ncells <- length(qcunit$x);
    nbrOfBytes <- 7*ncells;
    cells <- matrix(as.integer(c(qcunit$x, qcunit$y, qcunit$length,
                                 qcunit$pm, qcunit$background)),
                    ncol = 5)
    for(icell in seq(along.with = qcunit$x)) {
        writeBin(cells[icell, 1:2], con = con, size = 2, endian = "little")
        writeBin(cells[icell, 3:5], con = con, size = 1, endian = "little")
    }
    ## 3. Jump top front to write the qc position
    if(addPosition) {
        seek(con = con, where = positions$qcunitFP,
             origin = "start", rw = "write")
        writeBin(as.integer(endPosition),
                 con = con, size = 4, endian = "little")
        positions$qcunitFP <- positions$qcunitFP + 4
    }

    positions
} # writeCdfQcUnit()



writeCdf <- function(fname, cdfheader, cdf, cdfqc, overwrite = FALSE, verbose = 0) {
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Consistency checks
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (length(cdf) != cdfheader$nunits) {
      throw("Number of units in 'cdf' does not match the number of units in the CDF header: ", length(cdf), " != ", cdfheader$nunits);
    }

    if (length(cdfqc) != cdfheader$nqcunits) {
      throw("Number of QC units in 'cdfqc' does not match the number of QC units in the CDF header: ", length(cdfqc), " != ", cdfheader$nqcunits);
    }


    if(verbose >= 1) {
      cat("Writing CDF file...\n");
      cat("  Pathname: ", fname, "\n", sep="");
    }

    if(file.exists(fname) && !overwrite)
      stop("Cannot write CDF: File already exists: ", fname);

    # Open output connection (and make sure it is closed afterwards)
    cdfcon <- file(fname, open = "wb");
    on.exit(close(cdfcon));

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Create CDF and write unit names etc
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose >= 1)
      cat("  Writes CDF header and unit names...\n");

    if (cdfheader$nqcunits > 0) {
      # Start positions for QC units
      lens <- lapply(cdfqc, FUN=.subset2, "ncells");
      lens <- unlist(lens, use.names=FALSE);
      lens <- 6 + 7*lens;
      qcUnitLengths <- lens;
    } else {
      qcUnitLengths <- NULL;
    }

    if (cdfheader$nunits > 0) {
      # Start positions for units
      # Number of bytes: 20 + (18+64)*nbrOfBlocks + 14*totalNbrOfCells bytes
      lens <- lapply(cdf, FUN=function(unit) { 
        ncells <- .subset2(unit, "ncells");
        nblocks <- length(.subset2(unit, "blocks"));
        20 + 82*nblocks + 14*ncells;
      })
      lens <- unlist(lens, use.names=FALSE);
      unitLengths <- lens;
    } else {
      unitLengths <- NULL;
    }

    positions <- .initializeCdf(con = cdfcon,
                               nunits = cdfheader$nunits,
                               nqcunits = cdfheader$nqcunits,
                               refseq = cdfheader$refseq,
                               nrows = cdfheader$nrows,
                               ncol = cdfheader$ncols,
                               unitnames = names(cdf),
                               qcUnitLengths = qcUnitLengths,
                               unitLengths = unitLengths
    );
    if (verbose >= 1)
      cat("  Writes CDF header and unit names...done\n");

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Write all QC units
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose >= 1) {
        cat("  Writes QC units...\n");
      if(verbose >= 2)
        cat("    Units: ");
    }
    for(kk in seq(along.with = cdfqc)){
      if(verbose >= 2) {
        if (kk %% 100 == 0)
          cat(kk, ", ", sep="");
      }
      positions <- .writeCdfQcUnit(qcunit = cdfqc[[kk]], addPosition=FALSE,
                                  positions = positions,
                                  con = cdfcon)
    }
    if(verbose >= 2)
      cat("\n");
    if (verbose >= 1)
      cat("  Writes QC units...done\n");

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Write all units
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose >= 1) {
      cat("  Writes ", length(cdf), " units...\n", sep="");
      if(verbose >= 2)
        cat("    Units: ");
    }
    for(kk in seq(along.with = cdf)){
      if(verbose >= 2) {
        if (kk %% 100 == 0)
          cat(kk, ", ", sep="");
      }
      positions <- .writeCdfUnit(unit = cdf[[kk]], 
                                addName=FALSE, addPosition=FALSE,
                                unitname = names(cdf)[kk],
                                positions = positions,
                                con = cdfcon)
    }
    if(verbose >= 2)
      cat("\n");
    if (verbose >= 1)
      cat("  Writes ", length(cdf), " units...done\n", sep="");

    if(verbose >= 1)
      cat("Writing CDF file...done\n");

    invisible(NULL);
} # writeCdf()


############################################################################
# HISTORY:
# 2006-10-25 /HB (+KS)
# o BUG FIX: .initializeCdf() was writing false file offset for QC units
#   when the number QC nunits were zero.  This would core dump readCdfNnn().
# 2006-09-21 /HB
# o BUG FIX: The 'atom' and 'indexpos' fields were swapped.
# o Now suppressing warnings "writeChar: more characters requested..." in
#   writeCdf().
# 2006-09-11 /HB
# o BUG FIX: nrows & ncols were swapped in the CDF header.
# 2006-09-09 /HB
# o Updated writeCdf() has been validate with compareCdfs() on a few arrays.
# o With the below "optimizations" writeCdf() now writes Hu6800.CDF with
#   units in 130s compared to 140s.
# o Now initializeCdf() dumps all unit names at once by first building a
#   raw vector.  This is now much faster than before.
# o Now writeCdf() does not seek() around in the file anymore.  This should
#   speed up writing at least a bit.
# o Made some optimization, which speeds up the writing a bit.  Jumping
#   around in the file with seek() is expensive and should be avoided.
# o Rename writeUnit() to writeCdfUnit() and same for the QC function.
# o Added more verbose output and better errror messages for writeCdf().
# 2006-09-07 /HB
# o Maybe initalizeCdf(), writeUnit(), and writeQcUnit() should be made
#   private functions of this package.
# o Removed textCdf2binCdf() skeleton. See convertCdf() instead.
# o Updated writeCdf() such that the connection is guaranteed to be closed
#   regardless.
############################################################################

initializeCdf <- function(con, nrows = 1, ncols = 1,
                          nunits = 1, nqcunits = 0,
                          refseq = "",
                          unitnames = rep("", nunits),
                          qcunitpositions = rep(1, nqcunits),
                          unitpositions = rep(2, nunits),
                          ...) {
    if(length(unitpositions) != nunits)
        stop("unitpositions argument is not matching nunits")
    if(length(unitpositions) != nunits)
        stop("qcunitpositions argument is not matching nqcunits")
    if(length(refseq) != 1)
        stop("refseq argument is wrong")
    lrefseq <- nchar(refseq)
    ## Magic number and version number
    writeBin(object = as.integer(c(67, 1)),
             con = con, size = 4, endian = "little")
    ## Nrows, Ncols
    writeBin(object = as.integer(c(nrows, ncols)),
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
    ## Unit names
    writeChar(as.character(unitnames),
              nchars = rep(64, nunits), con = con, eos = NULL)
    ## QC units file positions
    writeBin(as.integer(qcunitpositions),
             con = con, size = 4, endian = "little")
    ## Units file positions
    writeBin(as.integer(unitpositions),
             con = con, size = 4, endian = "little")
    ## positions return argument
    offset <- 24 + lrefseq
    positions <- list(unitName = offset,
                      qcunitFP = offset + 64 * nunits,
                      unitFP = offset + 64 * nunits + 4 * nqcunits)
    return(positions)
} # initializeCdf()

writeUnit <- function(unit, con, unitname = NULL, positions = NULL,
                      addName = TRUE, addPosition = TRUE) {
    ## 1. Write the name in the start of the CDF file
    if(addName) {
        seek(con = con, where = positions$unitName,
             origin = "start")
        writeChar(as.character(unitname), nchars = 64,
                  con = con, eos = NULL)
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
    
    writeBin(unitInfo[1],
             con = con, size = 2, endian = "little")
    writeBin(unitInfo[2],
             con = con, size = 1, endian = "little")
    writeBin(unitInfo[3:6],
             con = con, size = 4, endian = "little")
    writeBin(unitInfo[7],
             con = con, size = 1, endian = "little")
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
        writeBin(blockInfo[1:2],
                 con = con, size = 4, endian = "little")
        writeBin(blockInfo[3:4],
                 con = con, size = 1, endian = "little")
        writeBin(blockInfo[5:6],
                 con = con, size = 4, endian = "little")
        writeChar(as.character(names(unit$blocks)[iblock]),
                  con = con, nchars = 64, eos = NULL) 
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
} # writeUnit()

writeQCUnit <- function(qcunit, con, positions = NULL,
                        addPosition = TRUE) {
    ## 1. Jump to end to write a qcunit
    seek(con = con, where = 0, origin = "end", rw = "write")
    endPosition <- seek(con = con, where = 0,
                        origin = "end", rw = "write")
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
    qcunitInfo <- as.integer(c(type, qcunit$ncells))
    writeBin(qcunitInfo[1], con = con, size = 2, endian = "little")
    writeBin(qcunitInfo[2], con = con, size = 4, endian = "little")

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
    if(!is.null(positions))
        return(positions)
    else 
        return(NULL)
} # writeQCUnit()



writeCdf <- function(fname, cdfheader, cdf, cdfqc, 
                     overwrite = FALSE, verbose = 0) {
    if(verbose)
        cat("Trying to create CDF file\n  ", fname, "\n")
    if(file.exists(fname) && !overwrite)
        stop("file", fname, "already exists")

    # Open output connection (and make sure it is closed afterwards)
    cdfcon <- file(fname, open = "wb");
    on.exit(close(cdfcon));

    # Create CDF and write header
    positions <- initializeCdf(con = cdfcon,
                               nunits = cdfheader$nunits,
                               nqcunits = cdfheader$nqcunits,
                               refseq = cdfheader$refseq,
                               nrows = cdfheader$nrows,
                               ncol = cdfheader$ncols);

    # Write all QC units
    for(i in seq(along.with = cdfqc)){
        if(verbose)
            cat("QC unit ", i, "\n")
        positions <- writeQCUnit(qcunit = cdfqc[[i]],
                                 positions = positions,
                                 con = cdfcon)
    }

    # Write all units
    for(i in seq(along.with = cdf)){
        if(verbose)
            cat(names(cdf)[i], "\n")
        positions <- writeUnit(unit = cdf[[i]],
                               unitname = names(cdf)[i],
                               positions = positions,
                               con = cdfcon)
    }

    if(verbose)
        cat("CDF file created.\n")

    return(invisible(NULL));
} # writeCdf()


############################################################################
# HISTORY:
# 2006-09-07 /HB
# o Maybe initalizeCdf(), writeUnit(), and writeQcUnit() should be made
#   private functions of this package.
# o Removed textCdf2binCdf() skeleton. See convertCdf() instead.
# o Updated writeCdf() such that the connection is guaranteed to be closed
#   regardless.
############################################################################

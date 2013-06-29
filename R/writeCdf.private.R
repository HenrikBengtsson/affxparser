.initializeCdf <- function(con, nRows = 1, nCols = 1,
                           nUnits = 1, nQcUnits = 0,
                           refSeq = "",
                           unitnames = rep("", nUnits),
                           qcUnitLengths = rep(0, nQcUnits),
                           unitLengths = rep(0, nUnits),
                           ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if(length(qcUnitLengths) != nQcUnits) {
    stop("Number of elements in argument 'qcUnitLengths' does not match 'nQcUnits'");
  }

  if(length(unitLengths) != nUnits) {
    stop("Number of elements in argument 'unitLengths' does not match 'nUnits'");
  }

  if(length(refSeq) != 1) {
    stop("Argument 'refSeq' should be a single character.");
  }

  lrefSeq <- nchar(refSeq);

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
  offset <- 0;

  ## Magic number and version number
  writeBin(object = as.integer(c(67, 1)),
           con = con, size = 4, endian = "little")
  ## NCols, NRows
  writeBin(object = as.integer(c(nCols, nRows)),
           con = con, size = 2, endian = "little")
  ## NumberUnits, NumberQCUnits
  writeBin(object = as.integer(c(nUnits, nQcUnits)),
           con = con, size = 4, endian = "little")
  ## Length of refSeqsequence
  writeBin(object = as.integer(lrefSeq),
           con = con, size = 4, endian = "little")
  offset <- 24;

  fOffset <- seek(con=con, origin="start", rw="write");
  if (offset != fOffset) {
    stop("File format write error (step 1): File offset is not the excepted one: ", fOffset, " != ", offset);
  }

  ## RefSeqsequece
  if(lrefSeq > 0)
    writeChar(as.character(refSeq), con=con, eos=NULL);

  # Current offset
  offset <- offset + lrefSeq;

  fOffset <- seek(con=con, origin="start", rw="write");
  if (offset != fOffset) {
    stop("File format write error (step 2): File offset is not the excepted one: ", fOffset, " != ", offset);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Unit names
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Write to raw vector (2*10^6 units => 122Mb; should be ok for now)
  # Since we can't create strings with '\0':s, we use '\xFF',
  # write to raw and then replace '\xFF' with '\0'. Thus, unit names with
  # '\xFF' are invalid, but this should not be a real problem.
  pads <- sapply(0:64, FUN=function(x) paste(rep("\xFF", x), collapse=""));

  # Write the unit names in chunks to save memory
  nbrOfUnits <- length(unitnames);
  chunkSize <- 100000;
  nbrOfChunks <- ceiling(nbrOfUnits / chunkSize);

  # Allocate raw vector
  raw <- raw(64*chunkSize);

  for (kk in 1:nbrOfChunks) {
    # Units for this chunk
    from <- (kk-1)*chunkSize+1;
    to <- min(from+chunkSize-1, nbrOfUnits);
    unitnamesFF <- unitnames[from:to];

    # Pad the unit names
    unitnamesFF <- paste(unitnamesFF, pads[64-nchar(unitnamesFF)], sep="");

    # Truncate last chunk?
    if (chunkSize > length(unitnamesFF)) {
      raw <- raw[1:(64*length(unitnamesFF))];
    }

    # Write unit names to raw vector
    raw <- writeBin(con=raw, unitnamesFF, size=1);
    unitnamesFF <- NULL; # Not needed anymore

    # Replace all '\xFF' with '\0'.
    idxs <- which(raw == as.raw(255));
    raw[idxs] <- as.raw(0);
    idxs <- NULL; # Not needed anymore

    writeBin(con=con, raw);
  } # for (kk in ...)
  raw <- NULL; # Not needed anymore

  bytesOfUnitNames <- 64 * nUnits;
  offset <- offset + bytesOfUnitNames;

  fOffset <- seek(con=con, origin="start", rw="write");
  if (offset != fOffset) {
    stop("File format write error (step 3): File offset is not the excepted one: ", fOffset, " != ", offset);
  }

  bytesOfQcUnits <- 4 * nQcUnits;
  offset <- offset + bytesOfQcUnits;

  bytesOfUnits <- 4 * nUnits;
  offset <- offset + bytesOfUnits;

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # QC units file positions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (nQcUnits > 0) {
    csum <- cumsum(qcUnitLengths);
    nextOffset <- csum[nQcUnits];
    starts <- c(0, csum[-nQcUnits]);
    starts <- as.integer(offset + starts);
    writeBin(starts, con = con, size = 4, endian = "little")
  } else {
    nextOffset <- 0;
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Units file positions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  offset <- offset + nextOffset;
  if (nUnits > 0) {
    csum <- cumsum(unitLengths);
    nextOffset <- csum[nUnits];
    starts <- c(0, csum[-nUnits]);
    starts <- as.integer(offset + starts);
    writeBin(starts, con = con, size = 4, endian = "little");
  } else {
    nextOffset <- 0;
  }
} # .initializeCdf()


.writeCdfUnit <- function(unit, con, unitname=NULL) {
  ## 3. Write the unit
  unitType <- unit$unittype
  if (!is.numeric(unitType)) {
    unitType <- switch(unitType,
                       unknown = 0,
                       expression = 1,
                       genotyping = 2,
                       resequencing = 3,
                       tag = 4,
                       copynumber = 5,
                       genotypingcontrol = 6,
                       expressioncontrol = 7)
  }

  unitDirection <- unit$unitdirection
  if (!is.numeric(unitDirection)) {
    unitDirection <- switch(unitDirection,
                            nodirection = 0,
                            sense = 1,
                            antisense = 2,
                            unknown = 3)
  }

  unitInfo <- as.integer(c(unitType, unitDirection,
                           unit$natoms, length(unit$groups),
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

  ## Writing each group in turn
  # Number of bytes: (18+64)*nbrOfGroups + 14*totalNbrOfCells bytes
  groupDirections <- c(nodirection=0, sense=1, antisense=2, unknown=3);
  for(igroup in seq(along.with = unit$groups)) {
    group <- unit$groups[[igroup]]
    groupDirection <- groupDirections[group$groupdirection];
    groupDirection <- switch(group$groupdirection,
                             nodirection = 0,
                             sense = 1,
                             antisense = 2,
                             unknown = 3)
    groupInfo <- as.integer(c(group$natoms, length(group$x),
                              group$ncellsperatom,
                              groupDirection, min(group$atoms, 0)))
    # Number of bytes: 2*4+2*1+2*4=18 bytes
    writeBin(groupInfo[1:2],
             con = con, size = 4, endian = "little")
    writeBin(groupInfo[3:4],
             con = con, size = 1, endian = "little")
    writeBin(groupInfo[5:6],
             con = con, size = 4, endian = "little")

    # Number of bytes: 64 bytes
    suppressWarnings({
      writeChar(as.character(names(unit$groups)[igroup]),
                con = con, nchars = 64, eos = NULL)
    })

    ## Writing each cell in turn
    cells <- matrix(as.integer(c(group$indexpos, group$x,
                                 group$y, group$atom)),
                    ncol = 4)

    # Number of bytes: 14*nbrOfCells bytes
    for(icell in seq(along.with = group$x)) {
      # Number of bytes: 1*4+2*2+1*4+1*2=14 bytes
      writeBin(cells[icell, 1],
               con = con, size = 4, endian = "little")
      writeBin(cells[icell, 2:3],
               con = con, size = 2, endian = "little")
      writeBin(cells[icell, 4],
               con = con, size = 4, endian = "little")
      writeChar(as.character(c(group$pbase[icell],
                               group$tbase[icell])),
                con = con, nchars = c(1,1), eos = NULL)
    } # for (icell ...)
  } # for (igroup ...)
} # .writeCdfUnit()



.writeCdfQcUnit <- function(qcunit, con) {
  ## 2. Actually write the qcunit
  type <- qcunit$type;
  if (!is.numeric(type)) {
    type <- switch(type,
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
  }

  # Write 2 + 4 bytes
  nbrOfBytes <- 6;
  qcunitInfo <- as.integer(c(type, qcunit$ncells))
  writeBin(qcunitInfo[1], con = con, size = 2, endian = "little")
  writeBin(qcunitInfo[2], con = con, size = 4, endian = "little")

  # Write 2 + 4 bytes
  nCells <- length(qcunit$x);
  nbrOfBytes <- 7*nCells;
  cells <- matrix(as.integer(c(qcunit$x, qcunit$y, qcunit$length,
                               qcunit$pm, qcunit$background)),
                  ncol = 5)
  for(icell in seq(along.with = qcunit$x)) {
    writeBin(cells[icell, 1:2], con = con, size = 2, endian = "little")
    writeBin(cells[icell, 3:5], con = con, size = 1, endian = "little")
  }
} # .writeCdfQcUnit()


############################################################################
# HISTORY:
# 2013-06-29
# o BUG FIX: Since affxparser 1.30.2/1.31.2, .writeCdfUnit() encoded unit
#   types incorrectly, iff specified as integers.
# o BUG FIX: Likewise, .writeCdfUnit() has always encoded unit directions
#   incorrectly, iff specified as integers.  Moreover, .writeCdfQcUnit()
#   has always encoded unit types incorrectly, iff specified as integers.
# 2013-05-25 /HB
# o Removed all gc() in .initializeCdf().
# 2013-01-07 /HB
# o GENERALIZATION: .writeCdfUnit() now also encodes unit types
#   'genotypingcontrol' and 'expressioncontrol'.
# o BUG FIX: .writeCdfUnit() incorrectly encoded the 'unknown' unit type
#   as 5 and not 0.
# 2008-08-09 /HB
# o BUG FIX: .writeCdfUnit() did output unit type 'resequencing' and 'tag'
#   as 4 and 3, and not 3 and 4, respectively.
# 2007-11-13 /KH
# o BUG FIX: The error message in internal .initializeCdf() would mention
#   'qcUnitLengths' when it was meant to say 'unitLengths'.
# 2007-07-13 /HB
# o While writing unit names in .initializeCdf(), quite a few copies were
#   created using up a lot of memory.  By removing unused objects and
#   writing unit names in chunks memory usage is now stable and < 200MB.
# 2007-02-01 /HB
# o Updated to camel case as much as possible to match JBs updates in the
#   branch.
# o Removed non-used arguments 'unitpositions' and 'qcunitpositions' from
#   .initializeCdf().
# 2007-01-10 /HB
# o Added writeCdfHeader(), writeCdfQcUnits() and writeCdfUnits().  With
#   these it is now possible to build up the CDF in chunks.
# o Removed obsolete arguments 'addName' and 'addPositions' and all related
#   code.  Internal variable 'positions' is not needed anymore.
#   There are no more seek():s in the code.
# o Removed obsolete .writeCdfUnit2().
# o Now only every 1000th unit (instead of 100th) is reported. It is now
#   also a count down.
# 2006-12-18 /KS
# o Make global replacement "block" -> "group" to maintain consistency
#   with other code, pursuant to communication from KH.
# 2006-10-25 /HB (+KS)
# o BUG FIX: .initializeCdf() was writing false file offset for QC units
#   when the number QC nUnits were zero.  This would core dump readCdfNnn().
# 2006-09-21 /HB
# o BUG FIX: The 'atom' and 'indexpos' fields were swapped.
# o Now suppressing warnings "writeChar: more characters requested..." in
#   writeCdf().
# 2006-09-11 /HB
# o BUG FIX: nRows & nCols were swapped in the CDF header.
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

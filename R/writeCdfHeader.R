#########################################################################/**
# @RdocFunction writeCdfHeader
#
# @title "Writes a CDF header"
#
# @synopsis 
# 
# \description{
#   @get "title".
#   \emph{This method is not indended to be used explicitly. 
#   To write a CDF, use @see "writeCdf" instead.}
# }
# 
# \arguments{
#   \item{con}{An open @connection to which nothing has been written.}
#   \item{cdfHeader}{A CDF header @list structure.}
#   \item{unitNames}{A @character @vector of all unit names.}
#   \item{qcUnitLengths}{An @integer @vector of all the number of bytes
#     in each of the QC units.}
#   \item{unitLengths}{An @integer @vector of all the number of bytes
#     in each of the (ordinary) units.}
#   \item{verbose}{An @integer specifying how much verbose details are
#     outputted.}
# }
# 
# \value{
#   Returns nothing.
# }
#
# @author "HB"
#
# \seealso{
#   This method is called by @see "writeCdf".
#   See also @see "writeCdfQcUnits" and @see "writeCdfUnits".
# }
# 
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*/#########################################################################
writeCdfHeader <- function(con, cdfHeader, unitNames, qcUnitLengths, unitLengths, verbose=0) {
  if (verbose >= 1)
    cat("  Writes CDF header and unit names...\n");

  .initializeCdf(con = con,
                 nRows = cdfHeader$nrows,
                 nCols = cdfHeader$ncols,
                 nUnits = cdfHeader$nunits,
                 nQcUnits = cdfHeader$nqcunits,
                 refSeq = cdfHeader$refseq,
                 unitnames = unitNames,
                 qcUnitLengths = qcUnitLengths,
                 unitLengths = unitLengths
  );

  if (verbose >= 1)
    cat("  Writes CDF header and unit names...done\n");
} # writeCdfHeader()


############################################################################
# HISTORY:
# 2007-02-01 /HB
# o Added Rdoc comments.
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

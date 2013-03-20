#########################################################################/**
# @RdocFunction writeCdfQcUnits
#
# @title "Writes CDF QC units"
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
#   \item{con}{An open @connection to which a CDF header already has
#     been written by @see "writeCdfHeader".}
#   \item{cdfQcUnits}{A @list structure of CDF QC units as returned by
#     @see "readCdf" (\emph{not} @see "readCdfUnits").}
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
#   See also @see "writeCdfHeader" and @see "writeCdfUnits".
# }
# 
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*/#########################################################################
writeCdfQcUnits <- function(con, cdfQcUnits, verbose=0) {
  if (verbose >= 1) {
    cat("  Writes QC units...\n");
    if(verbose >= 2)
      cat("    Units left: ");
  }

  nbrOfQCUnits <- length(cdfQcUnits);
  for(kk in seq(length=nbrOfQCUnits)) {
    if(verbose >= 2) {
      if (kk %% 1000 == 0)
        cat(nbrOfQCUnits-kk, ", ", sep="");
    }

    .writeCdfQcUnit(qcunit=.subset2(cdfQcUnits, kk), con=con);
  }
  if(verbose >= 2)
    cat("0\n");

  if (verbose >= 1)
    cat("  Writes QC units...done\n");
} # writeCdfQcUnits()



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

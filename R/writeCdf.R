writeCdf <- function(fname, cdfheader, cdf, cdfqc, overwrite=FALSE, verbose=0) {
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Consistency checks
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (length(cdf) != cdfheader$nunits) {
      stop("Number of units in 'cdf' does not match the number of units in the CDF header: ", length(cdf), " != ", cdfheader$nunits);
    }

    if (length(cdfqc) != cdfheader$nqcunits) {
      stop("Number of QC units in 'cdfqc' does not match the number of QC units in the CDF header: ", length(cdfqc), " != ", cdfheader$nqcunits);
    }


    if(verbose >= 1) {
      cat("Writing CDF file...\n");
      cat("  Pathname: ", fname, "\n", sep="");
    }

    if(file.exists(fname) && !overwrite)
      stop("Cannot write CDF: File already exists: ", fname);


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Figure out number of bytes per QC unit and regular unit
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
      # Number of bytes: 20 + (18+64)*nbrOfGroups + 14*totalNbrOfCells bytes
      lens <- lapply(cdf, FUN=function(unit) { 
        ncells <- .subset2(unit, "ncells");
        ngroups <- length(.subset2(unit, "groups"));
        20 + 82*ngroups + 14*ncells;
      })
      lens <- unlist(lens, use.names=FALSE);
      unitLengths <- lens;
    } else {
      unitLengths <- NULL;
    }


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Write CDF
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Open output connection
    con <- file(fname, open = "wb");
    on.exit(close(con));

    # Write CDF header
    writeCdfHeader(con=con, cdfheader, unitNames=names(cdf), 
                      qcUnitLengths=qcUnitLengths, unitLengths=unitLengths, 
                                                          verbose=verbose);

    # Write QC units
    writeCdfQcUnits(con=con, cdfqc, verbose=verbose);

    # Write regular units
    writeCdfUnits(con=con, cdf, verbose=verbose);


    if(verbose >= 1)
      cat("Writing CDF file...done\n");

    invisible(NULL);
} # writeCdf()



############################################################################
# HISTORY:
# 2012-05-18
# o Now using stop() instead of throw().
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

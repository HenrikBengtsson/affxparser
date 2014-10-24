readCel <- function(filename,
                    indices = NULL,
                    readHeader = TRUE,
                    readXY = FALSE,
                    readIntensities = TRUE,
                    readStdvs = FALSE,
                    readPixels = FALSE,
                    readOutliers = TRUE,
                    readMasked = TRUE,
                    readMap = NULL,
                    verbose = 0,
                    .checkArgs = TRUE) {
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Local functions
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    qsort <- function(x) {
  ##    o0 <- .Internal(qsort(x, TRUE));
  ##    o <- sort.int(x, index.return=TRUE, method="quick");
  ##    stopifnot(identical(o, o0));
      sort.int(x, index.return=TRUE, method="quick");
    } # qsort()


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Validate arguments
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    readAll <- is.null(indices);

    if (.checkArgs) {
      # Argument 'filename':
      if(length(filename) != 1) {
        stop("Argument 'filename' should be a single file: ",
                                              paste(filename, collapse=", "));
      }
      # Expand '~' pathnames to full pathnames.
      filename <- file.path(dirname(filename), basename(filename));
      if (!file.exists(filename)) {
        stop("Cannot read CEL file. File not found: ", filename);
      }

      # Argument 'indices':
      header <- readCelHeader(filename);
      nbrOfCells <- header$total;
      if (readAll) {
        # Read all cells
        indices <- 1:nbrOfCells;
      } else {
        indices <- as.integer(indices);
        if (any(is.na(indices))) {
          stop("Argument 'indices' contains NAs.");
        }
        if (any(indices < 1) || any(indices > nbrOfCells)) {
          stop("Argument 'indices' is out of range [1,", nbrOfCells, "].");
        }
      }

      # Argument 'readMap':
      if (!is.null(readMap)) {
        readMap <- .assertMap(readMap, nbrOfCells);
      }

      # Argument 'verbose':
      if (length(verbose) != 1) {
        stop("Argument 'verbose' must be a single integer.");
      }
      if (!is.finite(as.integer(verbose))) {
        stop("Argument 'verbose' must be an integer: ", verbose);
      }

    } # if (.checkArgs)


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Remapping cell indices?
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (is.null(readMap)) {
      # If not read map and no indices very given, then all cells are read
      # and already in order an no need to sort them.
      # *all cells are read
      reorder <- FALSE;
    } else {
      indices <- readMap[indices];
      reorder <- TRUE;
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Order cell indices for optimal speed when reading, i.e. minimizing
    # jumping around in the file.
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (reorder) {
      # qsort() is about 10-15 times faster than using order()!
      # WAS: o <- .Internal(qsort(indices, TRUE));  # From base::sort.int()
      o <- qsort(indices);
      indices <- o$x;
      # WAS: o <- .Internal(qsort(o$ix, TRUE))$ix;  # From base::sort.int()
      o <- qsort(o$ix)$ix;
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Reading CEL file
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # UNSUPPORTED CASE?
    if (!is.null(indices) && length(indices) == 0L) {
      stop("readCel(..., indices=integer(0)) is not supported.")
    }

    cel <- .Call("R_affx_get_cel_file", filename,
                  readHeader,
                  readIntensities, readXY, readXY, readPixels, readStdvs,
                  readOutliers, readMasked,
                  indices,
                  as.integer(verbose), PACKAGE="affxparser");

    # Sanity check
    if (is.null(cel)) {
      stop("Failed to read CEL file: ", filename);
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Re-reordering the cell values
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (reorder) {
      fields <- setdiff(names(cel), c("header", "outliers", "masked"));
      for (name in fields) {
        cel[[name]] <- cel[[name]][o];
      }
    }

    cel;
} # readCel()

############################################################################
# HISTORY:
# 2012-05-22 [HB]
# o CRAN POLICY: readCel() and readCelUnits() are no longer calling
#   .Internal(qsort(...)).
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2007-12-01
# o Removed argument 'reorder' from readCel().
# 2007-01-04
# o Now 'readMap' is validate using internal .assertMap(), which also
#   coerces it to an integer vector.
# o BUG FIX: Using read maps for readCel() would give an error saying
#   the read map is invalid even when it is not.
# 2006-04-01
# o Added argument 'reorder'.
# 2006-03-29
# o Added argument '.checkArgs' so that when arguments have already been
#   checked, for instance by readCelUnits(), we pay less overhead when
#   calling this function.  file.exists() is a bit slow. /HB
# 2006-03-28
# o Unit and cell indices are now one-based. /HB
############################################################################

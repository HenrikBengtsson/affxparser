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
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Validate arguments
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
      if (length(indices) == 0) {
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
        header <- readCelHeader(filename);
        nbrOfCells <- header$total;
        if (length(readMap) != nbrOfCells)
          stop("Argument 'readMap' is not a valid cell-index map.");
        if (match(1:nbrOfCells, readMap))
          stop("Argument 'readMap' is not a valid cell-index map.");
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
    if (!is.null(readMap)) {
      indices <- readMap[indices];
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Reading CEL file
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    .Call("R_affx_get_cel_file", filename,
          readHeader,
          readIntensities, readXY, readXY, readPixels, readStdvs,
          readOutliers, readMasked,
          indices,
          as.integer(verbose), PACKAGE="affxparser");
} # readCel()

############################################################################
# HISTORY:
# 2006-03-29
# o Added argument '.checkArgs' so that when arguments have already been
#   checked, for instance by readCelUnits(), we pay less overhead when
#   calling this function.  file.exists() is a bit slow. /HB
# 2006-03-28
# o Unit and cell indices are now one-based. /HB
############################################################################


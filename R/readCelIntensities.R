readCelIntensities <- function(filenames, indices = NULL, ..., verbose = 0){
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Validate arguments
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Argument 'filenames':
    if (length(filenames) == 0)
      stop("Argument 'filenames' is empty.");
    # Expand '~' pathnames to full pathnames.
    filenames <- file.path(dirname(filenames), basename(filenames));
    missing <- !file.exists(filenames);
    if(any(missing)) {
      missing <- paste(filenames[missing], collapse=", ");
      stop("Cannot read CEL files. Some files not found: ", missing);
    }

    # Argument 'verbose':
    if (length(verbose) != 1) {
      stop("Argument 'verbose' must be a single integer.");
    }
    if (!is.finite(as.integer(verbose))) {
      stop("Argument 'verbose' must be an integer: ", verbose);
    }
    verbose <- as.integer(verbose);


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Setup
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if(verbose > 0) {
      cat("Entering readCelIntensities()\n ... reading headers\n");
    }

    # Read all CEL headers
    all.headers <- lapply(as.list(filenames), readCelHeader)

    # Validate that all chips are of the same type and have the same layout
    chiptype <- unique(sapply(all.headers, function(x) x$chiptype))
    if(length(chiptype) != 1) {
      warning("The CEL files do not have the same chiptype.")
    }
    nrows <- unique(sapply(all.headers, function(x) x$rows))
    ncols <- unique(sapply(all.headers, function(x) x$cols))
    if(length(nrows) != 1 || length(ncols) != 1) {
      stop("The CEL files dimension do not match.");
    }

    nfiles <- length(filenames);
    if(verbose > 0) {
      cat(" ...allocating memory for intensity matrix\n");
    }

    # Allocating return matrix
    if(is.null(indices)) {
      intensities <- matrix(NA, nrow = nrows * ncols, ncol = nfiles)
    } else {
      intensities <- matrix(NA, nrow = length(indices), ncol = nfiles)
    }
    colnames(intensities) <- filenames


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Reading intensities from all CEL files
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    for (i in 1:nfiles) {
      if(verbose > 0)
        cat(" ... reading", filenames[i], "\n");

      intensities[, i] <- readCel(filename = filenames[i],
                                  indices = indices,
                                  readIntensities = TRUE,
                                  readHeader = FALSE,
                                  readStdvs = FALSE,
                                  readPixels = FALSE,
                                  readXY = FALSE,
                                  readOutliers = FALSE,
                                  readMasked = FALSE,
                                  ...,
                                  verbose = (verbose - 1))$intensities
    } # for (i in ...)

    intensities;
} # readCelIntensities()

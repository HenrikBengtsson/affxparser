readCel <- function(fname, indices = NULL,
                    readIntensities = TRUE, readHeader = TRUE,
                    readStdvs = FALSE, readPixels = FALSE,
                    readXY = FALSE, readOutliers = TRUE,
                    readMasked = TRUE, verbose = 0){
    ## Need sanity check on indices argument and other.

    # Argument 'fname':
    fname <- file.path(dirname(fname), basename(fname))
    if(length(fname) != 1)
        stop("The readCel function only reads a single cel file.")
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))


    # Argument 'indices':
    header <- readCelHeader(fname);
    if (length(indices) == 0)
      indices <- seq(from=0, to=header$total-1);
    indices <- as.integer(indices);
    if (any(is.na(indices)))
      stop("Argument 'indices' contains NAs.");
    if (any(indices < 0 | indices >= header$total))
      stop("Argument 'indices' is out of range [0,", header$total-1,"].");
    rm(header);

    # Argument 'verbose':
    if (length(verbose) != 1)
      stop("Argument 'verbose' must be a single integer.");
    verbose <- as.integer(verbose);
    if (!is.finite(verbose))
      stop("Argument 'verbose' must be an integer: ", verbose);

    cel.file <- .Call("R_affx_get_cel_file", fname,
                      readHeader = readHeader,
                      readIntensities = readIntensities,
                      readX = readXY,
                      readY = readXY,
                      readPixels = readPixels,
                      readStdvs = readStdvs,
                      readOutliers = readOutliers,
                      readMasked = readMasked,
                      indicesToRead = indices,
                      verbose = as.integer(verbose),
                      PACKAGE="affxparser")
    return(cel.file)
}

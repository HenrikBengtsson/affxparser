readCdfUnitNames <- function(filename, units=NULL, verbose=0) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'filename':
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename))
    stop("File not found: ", filename);

  # Argument 'units':
  if (is.null(units)) {
  } else if (is.numeric(units)) {
    units <- as.integer(units);
    if (any(units < 1))
      stop("Argument 'units' contains non-positive indices.");
  } else {
    stop("Argument 'units' must be numeric or NULL: ", class(units)[1]);
  }

  # Argument 'verbose':
  if (length(verbose) != 1)
    stop("Argument 'units' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'units' must be an integer: ", verbose);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # UNSUPPORTED CASE?
  if (!is.null(units) && length(units) == 0L) {
    stop("readCdfUnitNames(..., units=integer(0)) is not supported.")
  }

  res <- .Call("R_affx_get_cdf_unit_names", filename, units, verbose,
        PACKAGE="affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read unit names from CDF file: ", filename);
  }

  res;
} # readCdfUnitNames()

############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2006-03-28
# o Unit indices are now one-based. /HB
############################################################################

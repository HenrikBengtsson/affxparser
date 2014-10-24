#########################################################################/**
# @RdocFunction readCdfNbrOfCellsPerUnitGroup
#
# @title "Gets the number of cells (probes) that each group of each unit in a CDF file"
#
# @synopsis
#
# \description{
#   @get "title".
# }
#
# \arguments{
#  \item{filename}{The filename of the CDF file.}
#  \item{units}{An @integer @vector of unit indices specifying which units
#     to be read.  If @NULL, all units are read.}
#  \item{verbose}{An @integer specifying the verbose level. If 0, the
#     file is parsed quietly.  The higher numbers, the more details.}
# }
#
# \value{
#  A named @list of named @integer vectors.  The name of the list elements
#  are unit names and the names of the integer vector are group names.
# }
#
# @examples "../incl/readCdfNbrOfCellsPerUnitGroup.Rex"
#
# @author "HB"
#
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*/#########################################################################
readCdfNbrOfCellsPerUnitGroup <- function(filename, units=NULL, verbose=0) {
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
    stop("Argument 'verbose' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'verbose' must be an integer: ", verbose);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # UNSUPPORTED CASE?
  if (!is.null(units) && length(units) == 0L) {
    stop("readCdfNbrOfCellsPerUnitGroup(..., units=integer(0)) is not supported.")
  }

  res <- .Call("R_affx_cdf_nbrOfCellsPerUnitGroup", filename, units, verbose,
                                                     PACKAGE="affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read number of cells per unit group from CDF file: ", filename);
  }

  res;
} # readCdfNbrOfCellsPerUnitGroup()


############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2006-05-12
# o Added Rdoc comments (converted from Rd).
# 2006-03-28
# o Unit indices are now one-based. /HB
# 2006-01-11
# o Created. /HB
############################################################################

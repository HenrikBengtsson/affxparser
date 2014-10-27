#########################################################################/**
# @RdocFunction readCdfGroupNames
#
# @title "Reads group names for a set of units (probesets) in an Affymetrix CDF file"
#
# @synopsis
#
# \description{
#   @get "title".
#
#   This is for instance useful for SNP arrays where the nucleotides used
#   for the A and B alleles are the same as the group names.
# }
#
# \arguments{
#   \item{filename}{The filename of the CDF file.}
#   \item{units}{An @integer @vector of unit indices specifying which
#      units to be read.  If @NULL, all units are read.}
#    \item{truncateGroupNames}{A @logical variable indicating whether unit
#      names should be stripped from the beginning of group names.}
#    \item{verbose}{An @integer specifying the verbose level. If 0, the
#      file is parsed quietly.  The higher numbers, the more details.}
# }
#
# \value{
#    A named @list structure where the names of the elements are the names
#    of the units read.  Each element is a @character @vector with group
#    names for the corrsponding unit.
#  }
#
# @author "HB"
#
#  \seealso{
#    @see "readCdfUnits".
#  }
#
#
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCdfGroupNames <- function(filename, units=NULL, truncateGroupNames=TRUE, verbose=0) {
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

  # Argument 'truncateGroupNames':
  truncateGroupNames <- as.logical(truncateGroupNames);

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
    stop("readCdfGroupNames(..., units=integer(0)) is not supported.")
  }

  res <- .Call("R_affx_cdf_groupNames", filename, units,
        truncateGroupNames, verbose,
        PACKAGE="affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read unit group names from CDF file: ", filename);
  }

  res;
}


############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2007-03-05
# o Added argument truncateGroupNames. Also see R_affx_cdf_group_names(). /KS
# 2006-03-28
# o Unit indices are now one-based. /HB
# 2006-01-12
# o Created. /HB
############################################################################

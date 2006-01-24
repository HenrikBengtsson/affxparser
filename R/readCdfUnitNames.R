#########################################################################/**
# @RdocFunction readCdfUnitNames
# @alias read.cdf.unit.names
#
# @title "Reads unit (probeset) names from an Affymetrix CDF file"
#
# @synopsis 
# 
# \description{
#   Gets the names of all or a subset of units (probesets) in an 
#   Affymetrix CDF file.
#   This can be used to get a map between unit names an the internal unit
#   indices used by the CDF file.
# }
# 
# \arguments{
#   \item{filename}{The filename of the CDF file.}
#   \item{units}{An @integer @vector of (zero-based) unit indices 
#     specifying which units to be read.  If @NULL, all units are read.}
#   \item{verbose}{An @integer specifying the verbose level. If 0, the 
#     file is parsed quietly.  The higher numbers, the more details.}
# }
# 
# \value{
#   A @character @vector of unit names.
# }
# 
# \author{Henrik Bengtsson, \email{hb@maths.lth.se}.}
# 
# \seealso{
#   @see "readCdfUnits".
# }
# 
# \examples{\dontrun{See help(readCdfUnits) for an example}}
# 
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
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
    if (any(units < 0))
      stop("Argument 'units' contains negative indices.");
  } else {
    stop("Argument 'units' must be numeric or NULL: ", class(units)[1]);
  }

  # Argument 'units':
  if (length(verbose) != 1)
    stop("Argument 'units' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'units' must be an integer: ", verbose);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  .Call("R_affx_get_cdf_unit_names", filename, units, verbose,
                                                     PACKAGE="affxparser");
}

read.cdf.unit.names <- function(filename, units=NULL, verbose=0) {
  readCdfUnitNames(filename=filename, units=units, verbose=verbose);
}

############################################################################
# HISTORY:
# 2006-01-09
# o Note that ../man/readCdfUnitNames.R in generated from the Rdoc comments
#   above.  See the R.oo package for details.
# o Created by HB.  The purpose was to make it possible to read subsets
#   of units and not just all units at once. /HB
############################################################################

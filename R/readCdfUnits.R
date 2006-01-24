#########################################################################/**
# @RdocFunction readCdfUnits
# @alias read.cdf.units
#
# @title "Reads units (probesets) from an Affymetrix CDF file"
#
# @synopsis 
# 
# \description{
#   Gets the all or a subset of units (probesets) in an Affymetrix CDF file.
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
#   A named @list where the names corresponds to the names of the units
#   read.  Each element of the @list is in turn a @list structure 
#   with three components:
#    \item{groups}{A @list with one component for each group (also called
#      block). The information on each group is a @list with five 
#      components: \code{x}, \code{y}, \code{pbase}, \code{tbase},
#      \code{expos}.}
#    \item{type}{An @integer specifying the type of the unit.}
#    \item{direction}{An @integer specifying the direction of the unit.}
# }
#
# \author{James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
#   Daniel Hansen, \email{khansen@stat.berkeley.edu}.
#   Modified by Henrik Bengtsson, \email{hb@maths.lth.se}, to read
#   any subset of units.} 
# 
# \examples{\dontrun{@include "readCdfUnits.Rex"}}
# 
# \seealso{
#   @see "readCdfUnitNames".
# }
# 
# \references{
#   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
#       June 14, 2005.
#       \url{http://www.affymetrix.com/support/developer/}
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCdfUnits <- function(filename, units=NULL, verbose=0) {
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
  .Call("R_affx_get_cdf_units", filename, units, verbose, 
                                                     PACKAGE="affxparser");
}

read.cdf.units <- function(filename, units=NULL, verbose=0) {
  readCdfUnits(filename=filename, units=units, verbose=verbose);
}


############################################################################
# HISTORY:
# 2006-01-09
# o Note that ../man/readCdfUnits.R in generated from the Rdoc comments
#   above.  See the R.oo package for details.  Don't remove the *.Rex files!
# o Created by HB.  The purpose was to make it possible to read subsets
#   of units and not just all units at once.  /HB
############################################################################ 

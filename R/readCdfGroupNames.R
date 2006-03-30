#########################################################################/**
# @RdocFunction readCdfGroupNames
#
# @title "Reads group names of a set units (probesets) in an Affymetrix CDF file"
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
#  \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}
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
readCdfGroupNames <- function(filename, units=NULL, verbose=0) {
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
  .Call("R_affx_cdf_groupNames", filename, units, verbose,
                                                     PACKAGE="affxparser");
}


############################################################################
# HISTORY:
# 2006-03-28
# o Unit indices are now one-based. /HB
# 2006-01-12
# o Created. /HB
############################################################################

#########################################################################/**
# @RdocFunction invertMap
#
# @title "Inverts a read or a write map"
#
# @synopsis 
# 
# \description{
#   @get "title".
# }
# 
# \arguments{
#   \item{map}{An @integer @vector.}
#   \item{...}{Not used.}
# }
# 
# \value{
#   Returns an @integer @vector.
# }
#
# \details{
#   An map is defined to be a @vector of \emph{n} with unique finite
#   values in \eqn{[1,n]}.  Finding the inverse of a map is the same as 
#   finding the rank of each element, cf. @see "base::order".  However,
#   this method is much faster, because it utilizes the fact that all 
#   values are unique and in \eqn{[1,n]}.  Moreover, for any map it holds
#   that taking the inverse twice will result in the same map.
# }
#
# @examples "../incl/invertMap.Rex"
#
# @author "HB"
# 
# \seealso{
#   To generate an optimized write map for a CDF file, see 
#   @see "readCdfUnitsWriteMap".
# }
# 
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*/#########################################################################
invertMap <- function(map, ...) {
  n <- length(map);
  if (n == 0)
    return(map);

  # Validate map
  r <- range(map);
  if (any(is.na(r))) {
    stop("Argument 'map' is not a map. It contains NA values.");
  }
  if (r[1] != 1 || r[2] != n) {
    stop("Argument 'map' is not a map. Its range is not [1,", n, "]: ",
                                               "[", r[1], ", ", r[2], "]");
  }

  # Create the inverse map
  inverseMap <- vector("integer", n);
  idx <- 1:n;
  inverseMap[.subset(map, idx)] <- idx;  # == inverseMap[map[idx]] <- idx;

  inverseMap;
} # invertMap()


############################################################################
# HISTORY:
# 2006-03-30
# o Created.
############################################################################  

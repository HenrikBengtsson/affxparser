#########################################################################/-Rdoc TURNED OFF-**
# @RdocFunction .assertMap
#
# @title "Validates a read or a write map"
#
# @synopsis 
# 
# \description{
#   @get "title".
# }
# 
# \arguments{
#   \item{map}{An @integer @vector.}
#   \item{nbrOfCells}{The number of cells on the array.}
#   \item{...}{Not used.}
# }
# 
# \value{
#   Returns (invisibly) the map as an @integer @vector, if it is a valid
#   map, otherwise an error is thrown.
# }
#
# @author "HB"
# 
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*-Rdoc TURNED OFF-/#########################################################################
.assertMap <- function(map, nbrOfCells=length(map), ...) {
  n <- length(map);
  if (n != nbrOfCells) {
    stop("Argument 'map' is not a valid map. The number of elements does not match the number of cells on the array: ", n, " != ", nbrOfCells);
  }

  # Coerce to integers
  map <- as.integer(map);

  # Assert that there are no NAs
  r <- range(map);
  if (any(is.na(r))) {
    stop("Argument 'map' is not a valid map. It contains NA values.");
  }

  # Check range
  if (r[1] != 1 || r[2] != nbrOfCells) {
    stop("Argument 'map' is not a valid map. Its range is not [1,", 
                            nbrOfCells, "]: ", "[", r[1], ", ", r[2], "]");
  }

  # Check that the map is bijective
#  map2 <- invertMap(invertMap(map));
#  if (!identical(map, map2)) {
#    stop("Argument 'map' is not a valid map. It is not bijective.");
#  }
  
  invisible(map);
} # .assertMap()


############################################################################
# HISTORY:
# 2007-01-04
# o Created.
############################################################################  

########################################################################/**
# @RdocFunction cdfSetDimension
#
# @title "Sets the dimension of an object"
#
# \description{
#  @get "title".
#
#  This @function is designed to be used with @see "applyCdfGroupFields".
# }
#
# @synopsis
#
# \arguments{
#  \item{groups}{A @list of groups.}
#  \item{which}{An @integer or @character @vector of groups be returned.}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure of groups.
# }
#
# \seealso{
#  @see "applyCdfGroupFields".
# }
#
# @author "HB"
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfSetDimension <- function(field, dim, ...) {
  n <- length(field);
  ndim <- length(dim);

  naDim <- which(is.na(dim));
  if (length(naDim) > 0) {
    if (length(naDim) > 1) {
      stop("Cannot infer dimension. Only one of the dimension can be unknown: ", paste(dim, collapse="x"));
    }
    dimNA <- n / prod(dim[-naDim]);
    if (dimNA %% 1 == 0) {
      dim[naDim] <- dimNA;
      dim(field) <- dim;
    }
  } else if (n == prod(dim)) {
    dim(field) <- dim;
  }

  field;
} # cdfSetDimension()


############################################################################
# HISTORY:
# 2013-09-23
# o SPEEDUP/CLEANUP: Package now uses which() instead of whichVector()
#   of 'R.utils'.  Before R (< 2.11.0), which() used to be 10x slower
#   than whichVector(), but now it's 3x faster.
# 2009-05-29
# o Added Rdoc comments.
# o Created.
############################################################################

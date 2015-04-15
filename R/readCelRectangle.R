#########################################################################/**
# @RdocFunction readCelRectangle
#
# @title "Reads a spatial subset of probe-level data from Affymetrix CEL files"
#
# @synopsis
#
# \description{
#   @get "title".
# }
#
# \arguments{
#   \item{filename}{The pathname of the CEL file.}
#   \item{xrange}{A @numeric @vector of length two giving the left
#     and right coordinates of the cells to be returned.}
#   \item{yrange}{A @numeric @vector of length two giving the top
#     and bottom coordinates of the cells to be returned.}
#   \item{...}{Additional arguments passed to @see "readCel".}
#   \item{asMatrix}{If @TRUE, the CEL data fields are returned as
#     matrices with element (1,1) corresponding to cell
#     (xrange[1],yrange[1]).}
# }
#
# \value{
#   A named @list CEL structure similar to what @see "readCel".
#   In addition, if \code{asMatrix} is @TRUE, the CEL data fields
#   are returned as matrices, otherwise not.
# }
#
# @author "HB"
#
# @examples "../incl/readCelRectangle.Rex"
#
# \seealso{
#   The @see "readCel" method is used internally.
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCelRectangle <- function(filename, xrange=c(0,Inf), yrange=c(0,Inf), ..., asMatrix=TRUE) {
  # Argument 'xrange' and 'yrange':
  if (length(xrange) != 2) {
    stop("Argument 'xrange' is not of length 2: ", length(xrange))
  }

  if (length(yrange) != 2) {
    stop("Argument 'yrange' is not of length 2: ", length(yrange))
  }

  # Get the chip layout from the CEL header
  header <- readCelHeader(filename);
  nrow <- header$rows;
  ncol <- header$cols;

  xrange[1] <- max(min(xrange[1], ncol-1),0);
  xrange[2] <- max(min(xrange[2], ncol-1),0);
  yrange[1] <- max(min(yrange[1], nrow-1),0);
  yrange[2] <- max(min(yrange[2], nrow-1),0);

  yy <- yrange[1]:yrange[2];
  offsets <- yy * ncol + xrange[1];
  xrange <- xrange - xrange[1];
  xx <- xrange[1]:xrange[2];

  cells <- matrix(offsets, ncol=length(yy), nrow=length(xx), byrow=TRUE);
  # Cell indices are one-based in R
  cells <- cells + xx + 1;
  xrange <- yrange <- yy <- xx <- offsets <- NULL; # Not needed anymore

  # Read CEL data
  cel <- readCel(filename, indices=cells, ...);

  # Rearrange each field into a matrix?
  if (asMatrix) {
    nrow <- nrow(cells);
    cells <- NULL; # Not needed anymore

    # Cell-value fields
    fields <- c("x", "y", "intensities", "stdvs", "pixels");
    fields <- intersect(fields, names(cel));
    for (field in fields) {
      cel[[field]] <- matrix(cel[[field]], ncol=nrow, byrow=TRUE);
    }
  }

  cel;
} # readCelRectangle()

############################################################################
# HISTORY:
# 2014-10-24
# ROBUSTNESS: Now readCelRectangle() gives an informative error message
# if argument 'xrange' or 'yrange' is not of length two.
# 2006-03-28
# o Now cell indices are one-based.
# 2006-03-22
# o Added Rdoc comments.
# 2006-03-21 (Stockholm, Sveavagen)
# o Created.
############################################################################

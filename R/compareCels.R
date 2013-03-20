#########################################################################/**
# @RdocFunction compareCels
#
# @title "Compares the contents of two CEL files"
#
# @synopsis
#
# \description{
#   @get "title".
# }
#
# \arguments{
#   \item{pathname}{The pathname of the first CEL file.}
#   \item{other}{The pathname of the seconds CEL file.}
#   \item{readMap}{An optional read map for the first CEL file.}
#   \item{otherReadMap}{An optional read map for the second CEL file.}
#   \item{verbose}{An @integer. The larger the more details are printed.}
#   \item{...}{Not used.}
# }
#
# \value{
#   Returns @TRUE if the two CELs are equal, otherwise @FALSE.  If @FALSE,
#   the attribute \code{reason} contains a string explaining what
#   difference was detected, and the attributes \code{value1} and
#   \code{value2} contain the two objects/values that differs.
# }
#
# @author "HB"
#
# \seealso{
#   @see "convertCel".
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
compareCels <- function(pathname, other, readMap=NULL, otherReadMap=NULL, verbose=0, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  different <- function(fmtstr, ..., value1=NULL, value2=NULL) {
    res <- FALSE;
    attr(res, "reason") <- sprintf(fmtstr, ...);
    attr(res, "value1") <- value1;
    attr(res, "value2") <- value2;
    res;
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'pathname':
  # Expand any '~' in the pathname.
  pathname <- file.path(dirname(pathname), basename(pathname));
  if (!file.exists(pathname)) {
    stop("Cannot compare CELs. File not found: ", pathname);
  }
  # Expand any '~' in the pathname.
  other <- file.path(dirname(other), basename(other));
  if (!file.exists(other)) {
    stop("Cannot compare CELs. File not found: ", other);
  }

  # Argument 'verbose':
  verbose <- as.integer(verbose);


  if (verbose >= 1) {
    cat("Comparing CELs...\n");
    cat("  CEL 1: ", pathname, "\n", sep="");
    cat("  CEL 2: ", other, "\n", sep="");
  }


  if (verbose)
    cat("Reading first...\n");
  cel1 <- readCel(pathname, readHeader=TRUE, readIntensities=TRUE, readStdvs=TRUE, readPixels=TRUE, readOutliers=FALSE, readMasked=FALSE, readMap=readMap);
  if (verbose)
    cat("Reading first...done\n");

  if (verbose)
    cat("Reading second...\n");
    cel2 <- readCel(other, readHeader=TRUE, readIntensities=TRUE, readStdvs=TRUE, readPixels=TRUE, readOutliers=FALSE, readMasked=FALSE, readMap=otherReadMap);
  if (verbose)
    cat("Reading second...done\n");

  # Compare headers
  if (verbose >= 1)
    cat("  Comparing CEL headers...\n");
  excl <- c("filename", "version", "header", "datheader", "librarypackage");
  for (ff in setdiff(names(cel1$header), excl)) {
    h1 <- cel1$header[[ff]];
    h2 <- cel2$header[[ff]];
    if (!identical(h1, h2)) {
      (different("%s: %s != %s", ff, h1[[ff]], h2[[ff]]));
    }
  }
  if (verbose >= 1)
    cat("  Comparing CEL headers...done\n");

  # Compare data
  if (verbose >= 1)
    cat("  Comparing CEL data...\n");
  for (ff in c("intensities", "stdvs", "pixels")) {
    v1 <- cel1[[ff]];
    v2 <- cel2[[ff]];
    if (!identical(all.equal(v1, v2), TRUE)) {
      stop("Validation of new CEL file failed. Field differ: ", ff);
    }
  }
  if (verbose >= 1)
    cat("  Comparing CEL data...done\n");

  if (verbose >= 1)
    cat("Comparing CELs...done\n");

  TRUE;
} # compareCels()


############################################################################
# HISTORY:
# 2012-05-18
# o Now using stop() instead of throw().
# 2007-01-03
# o Created from compareCdfs.R.
############################################################################

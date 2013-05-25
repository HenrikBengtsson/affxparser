#########################################################################/**
# @RdocFunction compareCdfs
#
# @title "Compares the contents of two CDF files"
#
# @synopsis
#
# \description{
#   @get "title".
# }
#
# \arguments{
#   \item{pathname}{The pathname of the first CDF file.}
#   \item{other}{The pathname of the seconds CDF file.}
#   \item{quick}{If @TRUE, only a subset of the units are compared,
#     otherwise all units are compared.}
#   \item{verbose}{An @integer. The larger the more details are printed.}
#   \item{...}{Not used.}
# }
#
# \value{
#   Returns @TRUE if the two CDF are equal, otherwise @FALSE.  If @FALSE,
#   the attribute \code{reason} contains a string explaining what
#   difference was detected, and the attributes \code{value1} and
#   \code{value2} contain the two objects/values that differs.
# }
#
# \details{
#  The comparison is done with an upper-limit memory usage, regardless of
#  the size of the CDFs.
# }
#
# @author "HB"
#
# \seealso{
#   @see "convertCdf".
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
compareCdfs <- function(pathname, other, quick=FALSE, verbose=0, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  differentUnit <- function(value1, value2, units) {
    n <- length(units);

    # Done?
    if (n == 0) return(NULL);

    # Cannot narrow down?
    if (length(value1) != n) return(NULL);
    if (length(value2) != n) return(NULL);

    # Compare
    if (n == 1) {
      res <- all.equal(value1, value2);
      # Different?
      if (!identical(res, TRUE)) {
        return(units);
      } else {
        return(NULL);
      }
    }

    half <- floor(n/2);
    head <- 1:half;
    tail <- (half+1):n;

    # Among first half?
    unit <- differentUnit(value1[head], value2[head], units=units[head]);
    if (!is.null(unit)) return(unit);

    # Among second half?
    unit <- differentUnit(value1[tail], value2[tail], units=units[tail]);
    if (!is.null(unit)) return(unit);

    NULL;
  } # differentUnit()

  different <- function(fmtstr, ..., units=NULL, value1=NULL, value2=NULL) {
    res <- FALSE;
    attr(res, "reason") <- sprintf(fmtstr, ...);
    attr(res, "units") <- units;
    attr(res, "value1") <- value1;
    attr(res, "value2") <- value2;
    res;
  } # different()

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'pathname':
  # Expand any '~' in the pathname.
  pathname <- file.path(dirname(pathname), basename(pathname));
  if (!file.exists(pathname)) {
    stop("Cannot compare CDFs. File not found: ", pathname);
  }
  # Expand any '~' in the pathname.
  other <- file.path(dirname(other), basename(other));
  if (!file.exists(other)) {
    stop("Cannot compare CDFs. File not found: ", other);
  }

  # Argument 'verbose':
  verbose <- as.integer(verbose);


  if (verbose >= 1) {
    cat("Comparing CDFs...\n");
    cat("  CDF 1: ", pathname, "\n", sep="");
    cat("  CDF 2: ", other, "\n", sep="");
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Compare headers
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose >= 1)
    cat("  Comparing CDF headers...\n");
  h1 <- readCdfHeader(pathname);
  h2 <- readCdfHeader(other);
  for (ff in c("nrows", "ncols", "nunits", "nqcunits", "refseq")) {
    if (!identical(h1[[ff]], h2[[ff]]))
      return(different("%s: %s != %s", ff, h1[[ff]], h2[[ff]]));
  }
  if (verbose >= 1)
    cat("  Comparing CDF headers...done\n");

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Compare QC units
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose >= 1)
    cat("  Comparing QC units...\n");
  units <- seq(length=h1$nqcunits);
  while (length(units) > 0) {
    head <- 1:min(length(units),10);
    uu <- units[head];
    units <- units[-head];
    v1 <- readCdfQc(pathname, units=uu);
    v2 <- readCdfQc(other, units=uu);
    res <- all.equal(v1, v2);
    if (!identical(res, TRUE)) {
      badUnit <- differentUnit(value1=v1, value2=v2, units=uu);
      if (!is.null(badUnit)) {
        msg <- sprintf("Detected (at least one) QC unit that differ: %d", badUnit);
        units <- badUnit;
        idx <- match(badUnit, uu);
        v1 <- v1[idx];
        v2 <- v2[idx];
      } else {
        msg <- sprintf("Detected (at least one) QC unit that differ amount units %d to %d", min(uu), max(uu));
      }
      return(different(msg, units=units, value1=v1, value2=v2));
    }
    v1 <- v2 <- uu <- head <- NULL; # Not needed anymore
  }
  if (verbose >= 1)
    cat("  Comparing QC units...done\n");


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Compare units
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose >= 1) {
    cat("  Comparing units...\n");
    if (quick)
      cat("    Quick mode. Will only check a subset of the units...done\n");
  }
  if (verbose >= 2)
    cat("    Progress: ");
  units <- seq(length=h1$nunits);
  count <- 0;
  while (length(units) > 0) {
    head <- 1:min(length(units),500);
    if (verbose >= 2)
      cat(sprintf("%d%%, ", as.integer(100*count/h1$nunits)));
    uu <- units[head];
    units <- units[-head];
    v1 <- readCdf(pathname, units=uu);
    v2 <- readCdf(other, units=uu);
    res <- all.equal(v1, v2);
    if (!identical(res, TRUE)) {
      badUnit <- differentUnit(value1=v1, value2=v2, units=uu);
      if (!is.null(badUnit)) {
        msg <- sprintf("Detected (at least one) unit that differ: %d", badUnit);
        units <- badUnit;
        idx <- match(badUnit, uu);
        v1 <- v1[idx];
        v2 <- v2[idx];
      } else {
        msg <- sprintf("Detected (at least one) unit that differ amount units %d to %d", min(uu), max(uu));
      }
      return(different(msg, units=units, value1=v1, value2=v2));
    }
    count <- count + length(uu);
    if (quick)
      break;
    v1 <- v2 <- uu <- head <- NULL; # Not needed anymore
  }
  if (verbose >= 2)
    cat("100%.\n");
  if (verbose >= 1)
    cat("  Comparing units...done\n");

  if (verbose >= 1)
    cat("Comparing CDFs...done\n");

  TRUE;
} # compareCdfs()


############################################################################
# HISTORY:
# 2012-10-18
# o Now compareCdfs() gives a more precise 'reason' attribute when there
#   is a difference in (regular or QC) units.  It narrows down the first
#   unit that differs and reports it unit number.
# 2006-09-10
# o Added argument 'quick' to check only a subset of the units.
# 2006-09-09
# o Created.
############################################################################

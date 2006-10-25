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
# @author
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
    if (!identical(res, TRUE))
      return(different("QC units: ", value1=v1, value2=v2));
    rm(v1,v2,uu,head);
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
    if (!identical(res, TRUE))
      return(different("Units: ", value1=v1, value2=v2));
    count <- count + length(uu);
    if (quick)
      break;
    rm(v1,v2,uu,head);
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
# 2006-09-10
# o Added argument 'quick' to check only a subset of the units.
# 2006-09-09
# o Created.
############################################################################

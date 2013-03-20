########################################################################/**
# @RdocFunction findCdf
#
# @title "Search for CDF files in multiple directories"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{chipType}{A @character string of the chip type to search for.}
#  \item{paths}{A @character @vector of paths to be searched.  
#    The current directory is always searched at the beginning.
#    If @NULL, default paths are searched.  For more details, see below.}
#  \item{recursive}{If @TRUE, directories are searched recursively.}
#  \item{pattern}{A regular expression file name pattern to match.}
#  \item{...}{Additional arguments passed to @see "findFiles".}
# }
#
# \value{
#  Returns a @vector of the full pathnames of the files found.
# }
#
# \details{
#   Note, the current directory is always searched first, but never
#   recursively (unless it is added to the search path explicitly).
#   This provides an easy way to override other files in the search path.
#
#   If \code{paths} is @NULL, then a set of default paths are searched.
#   The default search path consitutes:
#   \enumerate{
#    \item \code{getOption("AFFX_CDF_PATH")}
#    \item \code{Sys.getenv("AFFX_CDF_PATH")}
#   }
#
#   One of the easiest ways to set system variables for \R is to
#   set them in an \code{.Renviron} file, e.g.
#   \preformatted{
#     # affxparser: Set default CDF path
#     AFFX_CDF_PATH=${AFFX_CDF_PATH};M:/Affymetrix_2004-100k_trios/cdf
#     AFFX_CDF_PATH=${AFFX_CDF_PATH};M:/Affymetrix_2005-500k_data/cdf
#   }
#   See @see "base::Startup" for more details.
# }
#
# @examples "../incl/findCdf.Rex"
#
# \seealso{
#   This method is used internally by @see "readCelUnits" if the CDF 
#   file is not specified.
# }
#
# @author "HB"
#
# @keyword file
# @keyword IO
#**/#######################################################################
findCdf <- function(chipType=NULL, paths=NULL, recursive=TRUE, pattern="[.](c|C)(d|D)(f|F)$", ...) {
  settings <- getOption("affxparser.settings");

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Customized search method?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  findFcn <- settings$methods$findCdf;
  if (!is.null(findFcn)) {
    if (!is.list(findFcn))
      findFcn <- list(findFcn);
    for (fcn in findFcn) {
      # Pass arguments as is
      pathnames <- fcn(chipType=chipType, paths=paths, 
                                recursive=recursive, pattern=pattern, ...);
      if (!is.null(pathnames))
        return(pathnames);
    }
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Setup search path and pattern
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (is.null(paths)) {
    paths <- c(getOption("AFFX_CDF_PATH"), Sys.getenv("AFFX_CDF_PATH"));
  }
  paths <- unlist(strsplit(paths, split=";"), use.names=FALSE);

  if (!is.null(chipType)) {
    if (regexpr("[.](c|C)(d|D)(f|F)$", chipType) !=-1)
      warning("Argument 'chipType' of findCdf() has suffix '.cdf':", chipType);
    pattern <- paste(chipType, pattern, sep="");
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Search
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # 1. First, search the current directory
  pathnames <- findFiles(pattern=pattern, paths=".", recursive=FALSE, ...);
  if (!is.null(pathnames))
    return(pathnames);

  # 2. Then, scan all of the search path
  findFiles(pattern=pattern, paths=paths, recursive=recursive, ...);
} 


############################################################################
# HISTORY:
# 2007-08-27
# o The current path is never scanned recursively (unless explicitly 
#   specified in the search path). This is to avoid endless scans in case
#   the search path has not been set.
# o Added argument 'recursive=TRUE' to findCdf().
# 2007-02-12 [moved to affxparser 2007-03-28]
# o Added option 'affxparser.settings' (for now private), allowing for an
#   alternative search function to be set it element methods$findCdf.
#   This is utilized by the aroma.affymetrix package to look for CDF 
#   using a more formalized hierarchical directory structure.
# 2006-09-21
# o findCdf() now gives a warning *.cdf extension is given, but tries not
#   to resolve it.
# 2006-09-15
# o The current directory "." was not the first directory scanned as
#   documented. Also added cdf/ and data/cdf to the beginning and not
#   at the end of the search path.
# 2006-03-14
# o Now making use of findFiles(), which makes this function much shorter.
# o Migrated the code to work without R.utils too.
# o Added system environment variable and option to the default path.
# 2006-02-22
# o Created.
############################################################################  

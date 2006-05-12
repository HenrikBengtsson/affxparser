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
#  \item{pattern}{A regular expression file name pattern to match.}
#  \item{...}{Additional arguments passed to @see "findFiles".}
# }
#
# \value{
#  Returns a @vector of the full pathnames of the files found.
# }
#
# \details{
#   Note, the current directory is always searched at the beginning.
#   This provides an easy way to override other files in the search path.
#   If \code{paths} is @NULL, then a set of default paths are searched.
#   The default search path is consituted of:
#   \enumerate{
#    \item \code{"."}
#    \item \code{getOption("AFFX_CDF_PATH")}
#    \item \code{Sys.getenv("AFFX_CDF_PATH")}
#    \item \code{"cdf/;data/cdf/"}
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
# @author
#
# @keyword file
# @keyword IO
#**/#######################################################################
findCdf <- function(chipType=NULL, paths=NULL, pattern="[.](c|C)(d|D)(f|F)$", ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'paths':
  if (is.null(paths)) {
    paths <- paste(".", 
                   getOption("AFFX_CDF_PATH"), 
                   Sys.getenv("AFFX_CDF_PATH"),
                   "cdf/", "data/cdf/", 
             sep=";", collapse=";");
  }

  # Argument 'chipType':
  if (!is.null(chipType)) {
    pattern <- paste(chipType, pattern, sep="");
  }


  findFiles(pattern=pattern, paths=paths, ...);
} 


############################################################################
# HISTORY:
# 2006-03-14
# o Now making use of findFiles(), which makes this function much shorter.
# o Migrated the code to work without R.utils too.
# o Added system environment variable and option to the default path.
# 2006-02-22
# o Created.
############################################################################  

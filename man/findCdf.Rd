%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  findCdf.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{findCdf}
\alias{findCdf}


\title{Search for CDF files in multiple directories}

\description{
 Search for CDF files in multiple directories.
}

\usage{
findCdf(chipType=NULL, paths=NULL, recursive=TRUE, pattern="[.](c|C)(d|D)(f|F)$", ...)
}

\arguments{
 \item{chipType}{A \code{\link[base]{character}} string of the chip type to search for.}
 \item{paths}{A \code{\link[base]{character}} \code{\link[base]{vector}} of paths to be searched.
   The current directory is always searched at the beginning.
   If \code{\link[base]{NULL}}, default paths are searched.  For more details, see below.}
 \item{recursive}{If \code{\link[base:logical]{TRUE}}, directories are searched recursively.}
 \item{pattern}{A regular expression file name pattern to match.}
 \item{...}{Additional arguments passed to \code{\link{findFiles}}().}
}

\value{
 Returns a \code{\link[base]{vector}} of the full pathnames of the files found.
}

\details{
  Note, the current directory is always searched first, but never
  recursively (unless it is added to the search path explicitly).
  This provides an easy way to override other files in the search path.

  If \code{paths} is \code{\link[base]{NULL}}, then a set of default paths are searched.
  The default search path consitutes:
  \enumerate{
   \item \code{getOption("AFFX_CDF_PATH")}
   \item \code{Sys.getenv("AFFX_CDF_PATH")}
  }

  One of the easiest ways to set system variables for \R is to
  set them in an \code{.Renviron} file, e.g.
  \preformatted{
    # affxparser: Set default CDF path
    AFFX_CDF_PATH=${AFFX_CDF_PATH};M:/Affymetrix_2004-100k_trios/cdf
    AFFX_CDF_PATH=${AFFX_CDF_PATH};M:/Affymetrix_2005-500k_data/cdf
  }
  See \code{\link[base]{Startup}} for more details.
}

\examples{
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Find a specific CDF file
cdfFile <- findCdf("Mapping10K_Xba131")
print(cdfFile)

# Find the first CDF file (no matter what it is)
cdfFile <- findCdf()
print(cdfFile)

# Find all CDF files in search path and display their headers
cdfFiles <- findCdf(firstOnly=FALSE)
for (cdfFile in cdfFiles) {
  cat("=======================================\n")
  hdr <- readCdfHeader(cdfFile)
  str(hdr)
}

##############################################################
}                                                     # STOP #
##############################################################

}

\seealso{
  This method is used internally by \code{\link{readCelUnits}}() if the CDF
  file is not specified.
}

\author{Henrik Bengtsson}



\keyword{file}
\keyword{IO}

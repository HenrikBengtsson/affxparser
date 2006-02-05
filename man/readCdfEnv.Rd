\name{readCdfEnv}
\alias{readCdfEnv}
\title{Creating an CDF environment using teh Fusion SDK}
\description{
  This function parses a CDF file and creates a corresponding CDF
  environment using the Afymetrix Fusion SDK.
}

\usage{
 readCdfEnv(fname, complementary.logic = TRUE, verbose = 0)
}

\arguments{
   \item{fname}{name of the CDF file.}
   \item{complementary.logic}{is a sanity check performed where we
     require that the mm match for a pm probe should be exactly the
     complementary of the pm probe. This should be the case for all cdf
     files except very special custom files (and SNP chip files).}
   \item{verbose}{how verbose do we want to be? 0 means no output, higher
     numbers is more verbose.}
}

\value{
   A cdf envirornment, for details see the link below.
}

\author{James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
   Daniel Hansen, \email{khansen@stat.berkeley.edu}}
\note{ ~~further notes~~ }
 
\seealso{\code{\link[makecdfenv]{make.cdf.env}} from \pkg{makecdfenv}
  for the default approach to creating cdf environments as well as
  \code{\link{affxparserInfo}} for general information about the Fusion
  SDK.}
\examples{
 \dontrun{
   readCdfEnv("test.CDF")
 }
}
\keyword{file}

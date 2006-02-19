\name{readCdf}
\alias{readCdf}
\title{Parsing a CDF file using Affymetrix Fusion SDK}
\description{
  This function parses a CDF file using the Affymetrix Fusion SDK.
}
\usage{
readCdf(fname, verbose = 0)
}
\arguments{
  \item{fname}{name of the CDF file.}
  \item{verbose}{how verbose should the output be. 0 means no output,
    with higher numbers being more verbose.}
}
\details{
}
\notes{
  This version of the function does not return information on the QC
  probes. This will be added in a (near) future release. In addition we
  expect the header to be part of the returned object.

  So expect changes to the structure of the value of the function in
  next release. Please contact the developers for details.
}
\value{
  A list with one component for each unit. Every component is again a
  list with three components    
  \item{groups}{This is again a list with one component for each group
    (also called block). The information on each group is a list with 5
    components, \code{x}, \code{y}, \code{pbase}, \code{tbase},
  \code{expos}.}
  \item{type}{type of the unit.}
  \item{direction}{direction of the unit.}
}
\author{James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
  Daniel Hansen, \email{khansen@stat.berkeley.edu}}
\note{}
\seealso{\code{\link{affxparserInfo}} for general information about the
  package/Fusion SDK and \code{\link{readCdfHeader}} for getting the
  header of a CDF file.}
\examples{
\dontrun{
readCdf("test.CDF")
}
}
\keyword{file}
\keyword{IO}

\name{readCdfHeader}
\alias{readCdfHeader}
\title{Reads the header associated with an Affymetrix CDF file}
\description{
Reads the header of an Affymetrix CDF file using the Fusion SDK. 
}
\usage{
readCdfHeader(fname)
}
\arguments{
  \item{fname}{name of the CDF file.}
}
\details{
}
\value{
  A named list with the following components
  \item{rows}{the number of rows on the chip.}
  \item{cols}{the number of columns on the chip.}
  \item{probesets}{the number of probesets on the chip.}
  \item{qcprobesets}{the number of QC probesets on the chip.}
  \item{reference}{the reference sequence (this component only exists
    for resequencing chips).}
  \item{chiptype}{the type of the chip.}
  \item{filename}{the name of the cdf file.}
  }
\references{}
\author{James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
  Daniel Hansen, \email{khansen@stat.berkeley.edu}}
\note{
}
\seealso{\code{\link{affxparserInfo}} for general information about the
  package and \code{\link{read.cdf.complete}} for information about CDF
  files.}
\examples{
\dontrun{
readCdfHeader(fname = "~/test.CDF")
}
}
\keyword{file}
\name{read.cdf.header}
\alias{read.cdf.header}
\title{Reads the header associated with an Affymetrix CDF file}
\description{
Reads the header of an Affymetrix CDF file using the Fusion SDK. A
convenience function.
}
\usage{
read.cdf.header(fname)
}
\arguments{
  \item{fname}{name of the CDF file.}
}
\details{
}
\value{
  A named list with the following components
  \item{chipName}{name of the associated chip.}
  \item{numberOfRows}{the number of rows for chips associated with the
    CDF file.}
  \item{numberOfColumns}{the number of columns for chips associated with
    the CDF file.}
  \item{numberOfProbesets}{}
  \item{numberOfQCProbesets}{}
  \item{referenceSequence}{the reference sequence. This slot is only
    non-empty if the chip in question is a resequencing chip.} 
}
\references{}
\author{James Bullard \email{bullard@stat.berkeley.edu} and Kasper
  Daniel Hansen \email{khansen@stat.berkeley.edu}}
\note{
}
\seealso{\code{\link{read.cdf.complete}} for the basic function.
\examples{
\dontrun{
read.cdf.header(fname = "~/test.CDF")
}
}
\keyword{file}
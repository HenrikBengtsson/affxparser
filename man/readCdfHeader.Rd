\name{readCdfHeader}
\alias{readCdfHeader}

\title{Reads the header associated with an Affymetrix CDF file}

\description{
 Reads the header of an Affymetrix CDF file using the Fusion SDK. 
}

\usage{
 readCdfHeader(filename)
}

\arguments{
  \item{filename}{name of the CDF file.}
}

\value{
  A named list with the following components:
    \item{rows}{the number of rows on the chip.}
    \item{cols}{the number of columns on the chip.}
    \item{probesets}{the number of probesets on the chip.}
    \item{qcprobesets}{the number of QC probesets on the chip.}
    \item{reference}{the reference sequence (this component only exists
      for resequencing chips).}
    \item{chiptype}{the type of the chip.}
    \item{filename}{the name of the cdf file.}
}

\examples{
for (zzz in 0) {

# Find any CDF file
cdfFile <- findCdf()
if (is.null(cdfFile))
  break

header <- readCdfHeader(cdfFile)
print(header)

} # for (zzz in 0)
}

\author{
  James Bullard, \email{bullard@stat.berkeley.edu} and 
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}
}

\seealso{
  \code{\link{readCdfUnits}()}.
}

\keyword{file}
\keyword{IO}

\name{readCdfQc}
\alias{readCdfQc}
\title{Reads the QC units of CDF file}

\description{
  Reads the QC units of CDF file.
}

\usage{
  readCdfQc(filename, units = NULL, verbose = 0)
}

\arguments{
  \item{filename}{name of the CDF file.}
  \item{units}{The QC unit indices as a vector of integers. \code{NULL}
  indicates that all units 
  should be read.}
  \item{verbose}{how verbose should the output be. 0 means no output,
    with higher numbers being more verbose.}
}

\value{
  A list with one component for each QC unit. 
}

\author{
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}
}

\seealso{
  \code{\link{readCdf}()}.
}

\keyword{file}
\keyword{IO}
\keyword{internal}

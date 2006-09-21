\name{writeCdf}
\alias{writeCdf}

\alias{initializeCdf}
\alias{writeCdfQcUnit}
\alias{writeCdfUnit}
\alias{writeUnit2}

\title{Creates a CDF file}

\description{
  This function creates a CDF file given a valid CDF structure
  containing all necessary elements.

  \emph{Warning: The API for this function is likely to be changed
    in future version.}
}

\usage{
  writeCdf(fname, cdfheader, cdf, cdfqc, overwrite=FALSE, verbose=0)
}

\arguments{
  \item{fname}{name of the CDF file.}
  \item{cdfheader}{A named list.}
  \item{cdf}{A named list.}
  \item{cdfqc}{A named list.}
  \item{overwrite}{Overwrite existing file?}
  \item{verbose}{how verbose should the output be. 0 means no output,
    with higher numbers being more verbose.}
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

\details{
  This function has been validated mainly by reading in various 
  ASCII or binary CDF files which are written back as new CDF 
  files, and compared element by element with the original files.
}

\author{
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}
}

\keyword{file}
\keyword{IO}
\keyword{internal}

\seealso{
  To read the CDF "regular" and QC units with all necessary fields 
  and values for writing a CDF file, see \code{\link{readCdf}}()
  and \code{\link{readCdfQc}}().
  To compare two CDF files, see \code{\link{compareCdfs}}().
}

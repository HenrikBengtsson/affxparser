\name{writeCdf}
\alias{writeCdf}

\title{Creates a binary CDF file}

\description{
  This function creates a binary CDF file given a valid CDF structure
  containing all necessary elements.

  \emph{Warning: The API for this function is likely to be changed
    in future versions.}
}

\usage{
  writeCdf(fname, cdfheader, cdf, cdfqc, overwrite=FALSE, verbose=0)
}

\arguments{
  \item{fname}{name of the CDF file.}
  \item{cdfheader}{A list with a structure equal to the output of \code{readCdfHeader}.}
  \item{cdf}{A list with a structure equal to the output of
  \code{readCdf}.} 
  \item{cdfqc}{A list with a structure equal to the output of \code{readCdfQc}.}
  \item{overwrite}{Overwrite existing file?}
  \item{verbose}{how verbose should the output be. 0 means no output,
    with higher numbers being more verbose.}
}

\details{
  This function has been validated mainly by reading in various 
  ASCII or binary CDF files which are written back as new CDF 
  files, and compared element by element with the original files.
}

\value{
  This function is used for its byproduct: creating a CDF file.
}

\author{
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}
}

\seealso{
  To read the CDF "regular" and QC units with all necessary fields 
  and values for writing a CDF file, see \code{\link{readCdf}},
  \code{\link{readCdfQc}}() and \code{\link{readCdfHeader}}.
  To compare two CDF files, see \code{\link{compareCdfs}}.
}


\keyword{file}
\keyword{IO}
\keyword{internal}

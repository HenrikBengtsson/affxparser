\name{readCelIntensities}
\alias{readCelIntensities}
\title{
  Reads the intensities contained in several Affymetrix CEL files
}
\description{
Reads the intensities of several Affymetrix CEL files (as opposed to
\code{readCel} which only reads a single file).
} 
\usage{
readCelIntensities(fnames, indices = NULL, verbose = 0)
}
\arguments{
\item{fnames}{the names of the CEL files as a character vector.}
\item{indices}{a vector of which indices should be read. If the argument
  is \code{NULL} all features will be returned.}
\item{verbose}{an integer: how verbose do we want to be, higher means
  more verbose.}
}
\details{
  The function will initially allocate a matrix with the same
  memory footprint as the final object.
}
\value{
  A matrix with a number of rows equal to the length of the
  \code{indices} argument (or the number of features on the entire
  chip), and a number of columns equal to the number of files. The
  columns are ordered according to the \code{fnames} argument.
}
\author{
  James Bullard, \email{bullard@stat.berkeley.edu} and
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}
}
\note{
  Currently this function builds on \code{read.cel.complete}, and simply
  calls this function multiple times. If testing yields sufficient
  reasons for doing so, it may be re-implemented in C++.
}
\seealso{
  \code{\link{readCel}} for the a discussion of a more
  versatile function, particular with details of the \code{indices}
  argument.
}
\examples{
\dontrun{
readCelIntensities(fnames = c("~/testFile1.CEL", "~/testFile2.CEL"))
}
}
\keyword{file}
\keyword{IO}

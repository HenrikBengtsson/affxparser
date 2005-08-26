\name{read.cel.intensity}
\alias{read.cel.intensity}
\title{Reads the intensities contained in several Affymetrix CEL files}
\description{
  Reads the intensities of several Affymetrix CEL files (as opposed to
  \code{read.cel.complete} which only reads a single file.
  }
\usage{
read.cel.intensity(fnames, indices = NULL, debug = FALSE)
}
\arguments{
  \item{fnames}{the names of the CEL files as a character vector.}
  \item{indices}{a vector of which indices should be read. If the
    argument is \code{NULL} all features will be returned.}
  \item{debug}{a logical: will debug details be echoed to the R
    prompt.}
}
\details{
  The function will initially allocate a matrix with the same memory
  footprint as the final object.
  }
\value{
  A matrix with a number of rows equal to the length of the \code{indices}
  argument (or the number of features on the entire chip}, and a number
of columns equal to the number of files. The columns are ordered
according to the \code{fnames} argument.
}
\author{James Bullard <bullard@stat.berkeley.edu>, Kasper Daniel Hansen
  <khansen@stat.berkeley.edu>} 
\note{
  Currently this function builds on \code{read.cel.complete}. It testing
  yields sufficient reasons for doing so, it may be re-implemented in
  C++.
}
\seealso{
  \code{\link{read.cel.complete}} for the basic function, including a
  discussion of the \code{indices} argument.
  } 
\examples{
\dontrun{
read.cel.intensity(fnames = c("~/testFile1.CEL", "~/testFile2.CEL"))
}
}
\keyword{file}

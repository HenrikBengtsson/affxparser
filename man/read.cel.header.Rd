\name{read.cel.header}
\alias{read.cel.header}
\title{Parsing the header of an Affymetrix CEL file}
\description{
  A convinience function build on \code{read.cel.complete}. Parses (part
  of) an Affymetrix CEL file using the Fusion SDK.
}
\usage{
read.cel.header(fname)
}
\arguments{
  \item{fname}{the name of the CEL file.}
}
\details{
  This function returns the header of a CEL file.
  }
\value{
  A named list with components described
  below. The entries are obtained from the Fusion SDK interface
  functions, hence the \code{numberOffeatures} component.
  \item{chipName}{the name of the chip}
  \item{numberOffeatures}{the number of features on the chip, should be
    equal to \code{numberOfRows} times \code{numberOfCols}.}
  \item{numberOfCols}{the number of columns on the chip.}
  \item{numberOfRows}{the number of rows on the chip.}
  \item{headerAsChar}{the header as a character. May contain information
    not present in the other slots.}
}
\author{James Bullard <bullard@stat.berkeley.edu>, Kasper Daniel Hansen
  <khansen@stat.berkeley.edu>} 
\note{
  Memory usage:the Fusion SDK allocates memory for the entire
  CEL file, when the file is accessed. The memory footprint of this
  function will therefore seem to be (rather) large.
}
\seealso{
  \code{\link{read.cel.complete}} for the basic function.
  } 
\examples{
\dontrun{
read.cel.header("~/testFile.CEL")
}
}
\keyword{file}

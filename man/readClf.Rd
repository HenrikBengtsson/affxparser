\name{readClf}
\alias{readClf}
\title{Parsing a CLF file using Affymetrix Fusion SDK}
\description{
 This function parses a CLF file using the Affymetrix Fusion SDK. CLF
 (chip layout) files contain information associating probe ids with chip
 x- and y- coordinates.
}
\usage{
readClf(file)
}
\arguments{
  \item{file}{\code{character(1)} providing a path to the CLF file to be
    input.}
}
\value{
  An list. The \code{header} element is always present.
  \item{header}{A list with information about the CLF file. The list
    contains elements described in the CLF file format document
    referenced below.}
  \item{dims}{A length-two integer vector of chip x- and y-coordinates.}
  \item{id}{An integer vector of length \code{prod(dims)} containing
    probe identifiers.}
  \item{x}{An integer vector of length \code{prod(dims)} containing
    x-coordinates corresponding to the entries in \code{id}.}
  \item{y}{An integer vector of length \code{prod(dims)} containing
    y-coordinates corresponding to the entries in \code{id}.}
}
\author{Martin Morgan \url{mtmorgan@fhcrc.org}}
\seealso{
  \url{https://www.affymetrix.com/support/developer/fusion/File_Format_CLF_aptv161.pdf}
  describes CLF file content.
}
\keyword{file}
\keyword{IO}

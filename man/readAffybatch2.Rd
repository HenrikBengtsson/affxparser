\name{readAffybatch2}
\alias{readAffybatch2}
\title{Read CEL files into an Affybatch}

\description{
Read CEL files into an Affybatch. An implemetation build on the
\pkg{affxparser} package.
}
\usage{
readAffybatch2(..., filenames = character(0),
               phenoData = new("phenoData"),
               description = NULL, notes = "",
               compress = getOption("BioC")$affy$compress.cel,
               rm.mask = FALSE, rm.outliers = FALSE,
               rm.extra = FALSE, verbose = FALSE)
}
\arguments{
  \item{\dots}{file names separated by comma.}
  \item{filenames}{file names in a character vector.}
  \item{phenoData}{a \code{phenoData} object}
  \item{description}{a \code{MIAME} object}
  \item{notes}{notes.}
  \item{compress}{Not currently supported.}
  \item{rm.mask}{Not currently supported.}
  \item{rm.outliers}{Not currently supported.}
  \item{rm.extra}{Not currently supported.}
  \item{verbose}{verbosity flag.}
}
\details{
}
\value{
  An \code{AffyBatch} object.
}
\author{The author of the original \code{read.affybatch} function:
  Ben Bolstad, \email{bolstad@stat.berkeley.edu} as well as 
  James Bullard, \email{bullard@stat.berkeley.edu} and Kasper Daniel Hansen
  \email{khansen@stat.berkeley.edu}
} 
\seealso{\code{\link[affy]{read.affybatch}} in \pkg{affy} as well as
  \code{\link{readCelIntensities}} and \code{\link{readCel}}.}
\examples{
}
\keyword{file}
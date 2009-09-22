\name{writeTpmap}
\alias{writeTpmap}
\alias{tpmap2bpmap}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{Writes BPMAP and TPMAP files.}
\description{
  Writes BPMAP and TPMAP files.
}
\usage{
writeTpmap(filename, bpmaplist, verbose = 0)

tpmap2bpmap(tpmapname, bpmapname, verbose = 0)
}
\arguments{
  \item{filename}{The filename.}
  \item{bpmaplist}{A list structure similar to the result of \code{readBpmap}.}
  \item{tpmapname}{Filename of the TPMAP file.}
  \item{bpmapname}{Filename of the BPMAP file.}
  \item{verbose}{How verbose do we want to be.}
}
\details{
  \code{writeTpmap} writes a text probe map file, while
  \code{tpmap2bpmap} converts such a file to a binary probe mapping
  file. Somehow Affymetrix has different names for the same structure,
  depending on whether the file is binary or text. I have seen many
  TPMAP files referred to as BPMAP files.
}
\value{
  These functions are called for their side effects (creating files).
}

\author{Kasper Daniel Hansen <khansen@stat.berkeley.edu>}

\seealso{\code{\link{readBpmap}}}

\keyword{file}
\keyword{IO}

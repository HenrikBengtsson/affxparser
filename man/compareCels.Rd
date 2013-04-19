%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  compareCels.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{compareCels}
\alias{compareCels}


\title{Compares the contents of two CEL files}

\usage{
compareCels(pathname, other, readMap=NULL, otherReadMap=NULL, verbose=0, ...)
}

\description{
  Compares the contents of two CEL files.
}

\arguments{
  \item{pathname}{The pathname of the first CEL file.}
  \item{other}{The pathname of the seconds CEL file.}
  \item{readMap}{An optional read map for the first CEL file.}
  \item{otherReadMap}{An optional read map for the second CEL file.}
  \item{verbose}{An \code{\link[base]{integer}}. The larger the more details are printed.}
  \item{...}{Not used.}
}

\value{
  Returns \code{\link[base:logical]{TRUE}} if the two CELs are equal, otherwise \code{\link[base:logical]{FALSE}}.  If \code{\link[base:logical]{FALSE}},
  the attribute \code{reason} contains a string explaining what
  difference was detected, and the attributes \code{value1} and
  \code{value2} contain the two objects/values that differs.
}

\author{Henrik Bengtsson}

\seealso{
  \code{\link{convertCel}}().
}



\keyword{file}
\keyword{IO}

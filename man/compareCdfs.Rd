%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  compareCdfs.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{compareCdfs}
\alias{compareCdfs}


\title{Compares the contents of two CDF files}

\usage{
compareCdfs(pathname, other, quick=FALSE, verbose=0, ...)
}

\description{
  Compares the contents of two CDF files.
}

\arguments{
  \item{pathname}{The pathname of the first CDF file.}
  \item{other}{The pathname of the seconds CDF file.}
  \item{quick}{If \code{\link[base:logical]{TRUE}}, only a subset of the units are compared,
    otherwise all units are compared.}
  \item{verbose}{An \code{\link[base]{integer}}. The larger the more details are printed.}
  \item{...}{Not used.}
}

\value{
  Returns \code{\link[base:logical]{TRUE}} if the two CDF are equal, otherwise \code{\link[base:logical]{FALSE}}.  If \code{\link[base:logical]{FALSE}},
  the attribute \code{reason} contains a string explaining what
  difference was detected, and the attributes \code{value1} and
  \code{value2} contain the two objects/values that differs.
}

\details{
 The comparison is done with an upper-limit memory usage, regardless of
 the size of the CDFs.
}

\author{Henrik Bengtsson}

\seealso{
  \code{\link{convertCdf}}().
}



\keyword{file}
\keyword{IO}

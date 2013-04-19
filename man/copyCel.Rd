%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  copyCel.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{copyCel}
\alias{copyCel}


\title{Copies a CEL file}

\description{
 Copies a CEL file.

 The file must be a valid CEL file, if not an exception is thrown.
}

\usage{
copyCel(from, to, overwrite=FALSE, ...)
}

\arguments{
 \item{from}{The filename of the CEL file to be copied.}
 \item{to}{The filename of destination file.}
 \item{overwrite}{If \code{\link[base:logical]{FALSE}} and the destination file already exists,
   an exception is thrown, otherwise not.}
 \item{...}{Not used.}
}

\value{
  Return \code{\link[base:logical]{TRUE}} if file was successfully copied, otherwise \code{\link[base:logical]{FALSE}}.
}

\seealso{
  \code{\link{isCelFile}}().
}

\author{Henrik Bengtsson}





\keyword{programming}
\keyword{file}
\keyword{IO}
\keyword{internal}

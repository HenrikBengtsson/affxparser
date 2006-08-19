%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  isCelFile.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{isCelFile}
\alias{isCelFile}


\title{Checks if a file is a CEL file or not}

\description{
 Checks if a file is a CEL file or not.
}

\usage{isCelFile(filename, ...)}

\arguments{
 \item{filename}{A filename.}
 \item{...}{Not used.}
}

\value{
 Returns \code{\link[base:logical]{TRUE}} if a CEL file, otherwise \code{\link[base:logical]{FALSE}}.
 If file does not exist, an exception is thrown.
}

\seealso{
 \code{\link{readCel}}(), \code{\link{readCelHeader}}(), \code{\link{readCelUnits}}().
}

\author{Henrik Bengtsson (\url{http://www.braju.com/R/})}





\keyword{programming}
\keyword{file}
\keyword{IO}
\keyword{internal}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  parseDatHeaderString.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{parseDatHeaderString}
\alias{parseDatHeaderString}


\title{Parses a DAT header string}

\description{
 Parses a DAT header string.
}

\usage{
parseDatHeaderString(header, timeFormat="\%m/\%d/\%y \%H:\%M:\%S", ...)
}

\arguments{
 \item{header}{A \code{\link[base]{character}} string.}
 \item{timeFormat}{The format string used to parse the timestamp.
   For more details, see \code{\link[base:strptime]{strptime}()}.
   If \code{\link[base]{NULL}}, no parsing is done.}
 \item{...}{Not used.}
}

\value{
 Returns named \code{\link[base]{list}} structure.
}

\seealso{
 \code{\link{readCelHeader}}().
}

\author{Henrik Bengtsson}





\keyword{programming}
\keyword{file}
\keyword{IO}
\keyword{internal}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfHeaderToCelHeader.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfHeaderToCelHeader}
\alias{cdfHeaderToCelHeader}


\title{Creates a valid CEL header from a CDF header}

\description{
 Creates a valid CEL header from a CDF header.
}

\usage{
cdfHeaderToCelHeader(cdfHeader, sampleName="noname", date=Sys.time(), ..., version="4")
}

\arguments{
 \item{cdfHeader}{A CDF \code{\link[base]{list}} structure.}
 \item{sampleName}{The name of the sample to be added to the CEL header.}
 \item{date}{The (scan) date to be added to the CEL header.}
 \item{...}{Not used.}
 \item{version}{The file-format version of the generated CEL file.
    Currently only version 4 is supported.}
}

\value{
 Returns a CDF \code{\link[base]{list}} structure.
}

\author{Henrik Bengtsson}



\keyword{programming}
\keyword{internal}

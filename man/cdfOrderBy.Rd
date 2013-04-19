%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfOrderBy.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfOrderBy}
\alias{cdfOrderBy}


\title{Orders the fields according to the value of another field in the same CDF group}

\description{
 Orders the fields according to the value of another field in the same CDF group.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{
cdfOrderBy(groups, field, ...)
}

\arguments{
 \item{groups}{A \code{\link[base]{list}} of groups.}
 \item{field}{The field whose values are used to order the other fields.}
 \item{...}{Optional arguments passed \code{\link[base]{order}}().}
}

\value{
 Returns a \code{\link[base]{list}} structure of groups.
}

\seealso{
 \code{\link{cdfOrderColumnsBy}}().
 \code{\link{applyCdfGroups}}().
}

\author{Henrik Bengtsson}



\keyword{programming}
\keyword{internal}

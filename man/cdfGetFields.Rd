%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  
% 
% on Wed Mar 29 20:22:50 2006.
% 
% Generator was the Rdoc class, which is part of the R.oo package written
% by Henrik Bengtsson, 2001-2006.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfGetFields}
\alias{cdfGetFields}


\title{Gets a subset of groups fields in a CDF structure}

\description{
 Gets a subset of groups fields in a CDF structure.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{cdfGetFields(groups, fields, ...)}

\arguments{
 \item{groups}{A \code{\link[base]{list}} of groups.}
 \item{fields}{A \code{\link[base]{character}} \code{\link[base]{vector}} of names of fields to be returned.}
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure of groups.
}

\details{
  Note that an error is \emph{not} generated for missing fields.
  Instead the field is returned with value \code{\link[base]{NA}}.  The reason for this
  is that it is much faster.
}

\seealso{
 \code{\link{applyCdfGroups}}().
}

\author{Henrik Bengtsson (\url{http://www.braju.com/R/})}


\keyword{programming}
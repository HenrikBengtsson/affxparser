%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  applyCdfGroupFields.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{applyCdfGroupFields}
\alias{applyCdfGroupFields}


\title{Applies a function to a list of fields of each group in a CDF structure}

\description{
 Applies a function to a list of fields of each group in a CDF structure.
}

\usage{
applyCdfGroupFields(cdf, fcn, ...)
}

\arguments{
 \item{cdf}{A CDF \code{\link[base]{list}} structure.}
 \item{fcn}{A \code{\link[base]{function}} that takes a \code{\link[base]{list}} structure of fields
            and returns an updated \code{\link[base]{list}} of fields.}
 \item{...}{Arguments passed to the \code{fcn} function.}
}

\value{
 Returns an updated CDF \code{\link[base]{list}} structure.
}

\author{Henrik Bengtsson}

\seealso{
  \code{\link{applyCdfGroups}}().
}


\keyword{programming}

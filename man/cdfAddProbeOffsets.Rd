%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  
% 
% on Tue Mar 14 18:46:09 2006.
% 
% Generator was the Rdoc class, which is part of the R.oo package written
% by Henrik Bengtsson, 2001-2006.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfAddProbeOffsets}
\alias{cdfAddProbeOffsets}


\title{Adds probe offsets to the groups in a CDF structure}

\description{
 Adds probe offsets to the groups in a CDF structure.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{cdfAddProbeOffsets(groups, ...)}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.}
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with half the number of groups as the
 \code{groups} argument (since allele A and allele B groups have
 been joined).
}

\details{
}

\seealso{
 \code{\link{applyCdfGroups}}().
}

\author{Henrik Bengtsson \url{http://www.braju.com/R/}}

\references{
 [1] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
     \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
}


\keyword{programming}

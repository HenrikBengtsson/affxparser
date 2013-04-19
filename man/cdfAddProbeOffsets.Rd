%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfAddProbeOffsets.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfAddProbeOffsets}
\alias{cdfAddProbeOffsets}


\title{Adds probe offsets to the groups in a CDF structure}

\description{
 Adds probe offsets to the groups in a CDF structure.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{
cdfAddProbeOffsets(groups, ...)
}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.
   Each group must contain the fields \code{tbase}, and \code{expos}.
 }
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with half the number of groups as the
 \code{groups} argument (since allele A and allele B groups have
 been joined).
}

\seealso{
 \code{\link{applyCdfGroups}}().
}

\author{Henrik Bengtsson}

\references{
 [1] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
     \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
}



\keyword{programming}
\keyword{internal}

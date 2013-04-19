%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfMergeStrands.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfMergeStrands}
\alias{cdfMergeStrands}


\title{Function to join CDF groups with the same names}

\description{
 Function to join CDF groups with the same names.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.

 This can be used to join the sense and anti-sense groups of the same
 allele in SNP arrays.
}

\usage{
cdfMergeStrands(groups, ...)
}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.}
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with only two groups.
}

\details{
 If a unit has two strands, they are merged such that the elements
 for the second strand are concattenated to the end of the elements
 of first strand (This is done seperately for the two alleles).
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

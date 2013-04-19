%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfMergeAlleles.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfMergeAlleles}
\alias{cdfMergeAlleles}


\title{Function to join CDF allele A and allele B groups strand by strand}

\description{
 Function to join CDF allele A and allele B groups strand by strand.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{
cdfMergeAlleles(groups, compReverseBases=FALSE, collapse="", ...)
}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.}
 \item{compReverseBases}{If \code{\link[base:logical]{TRUE}}, the group names, which typically are
   names for bases, are turned into their complementary bases for the
   reverse strand.}
 \item{collapse}{The \code{\link[base]{character}} string used to collapse the allele A
   and the allele B group names.}
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with the two groups \code{forward}
 and \code{reverse}, if the latter exists.
}

\details{
 Allele A and allele B are merged into a \code{\link[base]{matrix}} where first row
 hold the elements for allele A and the second elements for allele B.
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

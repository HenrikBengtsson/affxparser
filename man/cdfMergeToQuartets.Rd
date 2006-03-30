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

\name{cdfMergeToQuartets}
\alias{cdfMergeToQuartets}


\title{Function to re-arrange CDF groups values in quartets}

\description{
 Function to re-arrange CDF groups values in quartets.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.

 Note, this requires that the group values have already been
 arranged in PMs and MMs.
}

\usage{cdfMergeToQuartets(groups, ...)}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.}
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with the two groups \code{forward}
 and \code{reverse}, if the latter exists.
}

\seealso{
 \code{\link{applyCdfGroups}}().
}

\author{Henrik Bengtsson (\url{http://www.braju.com/R/})}

\references{
 [1] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
     \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
}


\keyword{programming}

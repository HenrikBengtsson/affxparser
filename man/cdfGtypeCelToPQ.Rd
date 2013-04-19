%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfGtypeCelToPQ.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfGtypeCelToPQ}
\alias{cdfGtypeCelToPQ}


\title{Function to immitate Affymetrix' gtype\_cel\_to\_pq software}

\description{
 Function to immitate Affymetrix' gtype\_cel\_to\_pq software.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{
cdfGtypeCelToPQ(groups, ...)
}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.}
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with a single group.  The fields in this
 groups are in turn vectors (all of equal length) where the elements
 are stored as subsequent quartets (PMA, MMA, PMB, MMB) with all
 forward-strand quartets first followed by all reverse-strand quartets.
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

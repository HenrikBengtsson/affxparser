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

\name{cdfGtypeCelToPQ}
\alias{cdfGtypeCelToPQ}


\title{Function to immitate Affymetrix' gtype\_cel\_to\_pq software}

\description{
 Function to immitate Affymetrix' gtype\_cel\_to\_pq software.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{cdfGtypeCelToPQ(groups, ...)}

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
 \code{\link{gtypeCelToPQ}}.
 \code{\link{applyCdfGroups}}().
}

\author{Henrik Bengtsson \url{http://www.braju.com/R/}}

\references{
 [1] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
     \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
}


\keyword{programming}

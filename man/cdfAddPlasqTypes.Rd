%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfAddPlasqTypes.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfAddPlasqTypes}
\alias{cdfAddPlasqTypes}


\title{Adds the PLASQ types for the probes in a CDF structure}

\description{
 Adds the PLASQ types for the probes in a CDF structure.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
}

\usage{
cdfAddPlasqTypes(groups, ...)
}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.
   Each group must contain the fields \code{tbase}, \code{pbase},
   and \code{expos}.
 }
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with the same number of groups as the
 \code{groups} argument.  To each group, one fields is added:
  \item{plasqType}{A \code{\link[base]{vector}} of \code{\link[base]{integer}}s in [0,15].}
}

\details{
 This function identifies the number of nucleotides (bases) in probe
 sequences that mismatch the the target sequence for allele A and the
 allele B, as used by PLASQ [1], and adds an integer [0,15] interepreted
 as one of 16 probe types.  In PLASQ these probe types are referred to as:
 0=MMoBR, 1=MMoBF, 2=MMcBR, 3=MMcBF, 4=MMoAR, 5=MMoAF, 6=MMcAR, 7=MMcAF,
 8=PMoBR, 9=PMoBF, 10=PMcBR, 11=PMcBF, 12=PMoAR, 13=PMoAF, 14=PMcAR,
 15=PMcAF.\cr

 Pseudo rule for finding out the probe-type value:\cr
 \itemize{
  \item PM/MM: For MMs add 0, for PMs add 8.
  \item A/B: For Bs add 0, for As add 4.
  \item o/c: For shifted (o) add 0, for centered (c) add 2.
  \item R/F: For antisense (R) add 0, for sense (F) add 1.
 }
 Example: (PM,A,c,R) = 8 + 4 + 2 + 0 = 14 (=PMcAR)
}

\author{Henrik Bengtsson}

\references{
 [1] LaFramboise T, Weir BA, Zhao X, Beroukhim R, Li C, Harrington D,
     Sellers WR, and Meyerson M. \emph{Allele-specific amplification in
     cancer revealed by SNP array analysis}, PLoS Computational Biology,
     Nov 2005, Volume 1, Issue 6, e65.\cr
}



\keyword{programming}
\keyword{internal}

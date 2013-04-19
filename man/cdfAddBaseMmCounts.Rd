%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfAddBaseMmCounts.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{cdfAddBaseMmCounts}
\alias{cdfAddBaseMmCounts}


\title{Adds the number of allele A and allele B mismatching nucleotides of the probes in a CDF structure}

\description{
 Adds the number of allele A and allele B mismatching nucleotides of the probes in a CDF structure.

 This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
 on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.

 Identifies the number of nucleotides (bases) in probe sequences that
 mismatch the the target sequence for allele A and the allele B,
 as used by [1].
}

\usage{
cdfAddBaseMmCounts(groups, ...)
}

\arguments{
 \item{groups}{A \code{\link[base]{list}} structure with groups.
   Each group must contain the fields \code{tbase}, \code{pbase}, and
   \code{offset} (from \code{\link{cdfAddProbeOffsets}}()).
 }
 \item{...}{Not used.}
}

\value{
 Returns a \code{\link[base]{list}} structure with the same number of groups as the
 \code{groups} argument.  To each group, two fields is added:
  \item{mmACount}{The number of nucleotides in the probe sequence
     that mismatches the target sequence of allele A.}
  \item{mmBCount}{The number of nucleotides in the probe sequence
     that mismatches the target sequence of allele B.}
}

\details{
 Note that the above counts can be inferred from the CDF structure alone,
 i.e. no sequence information is required.
 Consider a probe group interrogating allele A.  First, all PM probes
 matches the allele A target sequence perfectly regardless of shift.
 Moreover, all these PM probes mismatch the allele B target sequence
 at exactly one position.  Second, all MM probes mismatches the
 allele A sequence at exactly one position.  This is also true for
 the allele B sequence, \emph{except} for an MM probe with zero offset,
 which only mismatch at one (the middle) position.
 For a probe group interrogating allele B, the same rules applies with
 labels A and B swapped.
 In summary, the mismatch counts for PM probes can take values 0 and 1,
 and for MM probes they can take values 0, 1, and 2.
}

\seealso{
 To add required probe offsets, \code{\link{cdfAddProbeOffsets}}().
 \code{\link{applyCdfGroups}}().
}

\author{Henrik Bengtsson}

\references{
 [1] LaFramboise T, Weir BA, Zhao X, Beroukhim R, Li C, Harrington D,
     Sellers WR, and Meyerson M. \emph{Allele-specific amplification in
     cancer revealed by SNP array analysis}, PLoS Computational Biology,
     Nov 2005, Volume 1, Issue 6, e65.\cr
 [2] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
     \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
}



\keyword{programming}
\keyword{internal}

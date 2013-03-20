########################################################################/**
# @RdocFunction cdfAddBaseMmCounts
#
# @title "Adds the number of allele A and allele B mismatching nucleotides of the probes in a CDF structure"
#
# \description{
#  @get "title".
#
#  This @function is design to be used with @see "applyCdfGroups"
#  on an Affymetrix Mapping (SNP) CDF @list structure.
#
#  Identifies the number of nucleotides (bases) in probe sequences that
#  mismatch the the target sequence for allele A and the allele B,
#  as used by [1].
# }
#
# @synopsis
#
# \arguments{
#  \item{groups}{A @list structure with groups.  
#    Each group must contain the fields \code{tbase}, \code{pbase}, and 
#    \code{offset} (from @see "cdfAddProbeOffsets").
#  }
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure with the same number of groups as the 
#  \code{groups} argument.  To each group, two fields is added:
#   \item{mmACount}{The number of nucleotides in the probe sequence 
#      that mismatches the target sequence of allele A.}
#   \item{mmBCount}{The number of nucleotides in the probe sequence 
#      that mismatches the target sequence of allele B.}
# }
#
# \details{
#  Note that the above counts can be inferred from the CDF structure alone, 
#  i.e. no sequence information is required.
#  Consider a probe group interrogating allele A.  First, all PM probes
#  matches the allele A target sequence perfectly regardless of shift.
#  Moreover, all these PM probes mismatch the allele B target sequence 
#  at exactly one position.  Second, all MM probes mismatches the
#  allele A sequence at exactly one position.  This is also true for
#  the allele B sequence, \emph{except} for an MM probe with zero offset,
#  which only mismatch at one (the middle) position.
#  For a probe group interrogating allele B, the same rules applies with
#  labels A and B swapped.
#  In summary, the mismatch counts for PM probes can take values 0 and 1,
#  and for MM probes they can take values 0, 1, and 2.
# }
#
# \seealso{
#  To add required probe offsets, @see "cdfAddProbeOffsets".
#  @see "applyCdfGroups".
# }
#
# @author "HB"
#
# \references{
#  [1] LaFramboise T, Weir BA, Zhao X, Beroukhim R, Li C, Harrington D, 
#      Sellers WR, and Meyerson M. \emph{Allele-specific amplification in 
#      cancer revealed by SNP array analysis}, PLoS Computational Biology,
#      Nov 2005, Volume 1, Issue 6, e65.\cr
#  [2] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
#      \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
# }
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfAddBaseMmCounts <- function(groups, ...) {
  for (gg in seq(along=groups)) {
    group <- groups[[gg]];

    # Find PM probes
    tbase <- group$tbase;
    pbase <- group$pbase;
    isPm <- ((tbase == "a" | tbase == "A") &
             (pbase == "t" | pbase == "T")) |
            ((tbase == "t" | tbase == "T") &  
             (pbase == "a" | pbase == "A")) |
            ((tbase == "c" | tbase == "C") &  
             (pbase == "g" | pbase == "G")) |
            ((tbase == "g" | tbase == "G") &
             (pbase == "c" | pbase == "C"));

    # Find the center probes
    isCentered <- (group$offset == 0);

    dim <- dim(isCentered);
    if (is.null(dim)) {
      mmACount <- mmBCount <- rep(as.integer(1), length(isCentered));
    } else {
      mmACount <- mmBCount <- array(as.integer(1), dim=dim,
                                            dimnames=dimnames(isCentered));
    }
    
    # Is this probe group interrogating allele A?
    isA <- (gg %% 2 == 1);
    if (isA) {
      mmACount[isPm] <- as.integer(0);
      mmBCount[!isPm & !isCentered] <- as.integer(2);
    } else {
      mmBCount[isPm] <- as.integer(0);
      mmACount[!isPm & !isCentered] <- as.integer(2);
    }

    # Add the new fields
    group$mmACount <- mmACount;
    group$mmBCount <- mmBCount;

    groups[[gg]] <- group;
  }

  groups;
}

############################################################################
# HISTORY:
# 2006-06-19
# o Added more Rdoc help.
# 2006-03-07
# o Created.
############################################################################  

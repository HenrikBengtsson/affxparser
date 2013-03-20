########################################################################/**
# @RdocFunction cdfAddPlasqTypes
#
# @title "Adds the PLASQ types for the probes in a CDF structure"
#
# \description{
#  @get "title".
#
#  This @function is design to be used with @see "applyCdfGroups"
#  on an Affymetrix Mapping (SNP) CDF @list structure.
# }
#
# @synopsis
#
# \arguments{
#  \item{groups}{A @list structure with groups. 
#    Each group must contain the fields \code{tbase}, \code{pbase}, 
#    and \code{expos}.
#  }
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure with the same number of groups as the 
#  \code{groups} argument.  To each group, one fields is added:
#   \item{plasqType}{A @vector of @integers in [0,15].}
# }
#
# \details{
#  This function identifies the number of nucleotides (bases) in probe
#  sequences that mismatch the the target sequence for allele A and the
#  allele B, as used by PLASQ [1], and adds an integer [0,15] interepreted
#  as one of 16 probe types.  In PLASQ these probe types are referred to as:
#  0=MMoBR, 1=MMoBF, 2=MMcBR, 3=MMcBF, 4=MMoAR, 5=MMoAF, 6=MMcAR, 7=MMcAF, 
#  8=PMoBR, 9=PMoBF, 10=PMcBR, 11=PMcBF, 12=PMoAR, 13=PMoAF, 14=PMcAR, 
#  15=PMcAF.\cr
#
#  Pseudo rule for finding out the probe-type value:\cr
#  \itemize{
#   \item PM/MM: For MMs add 0, for PMs add 8.
#   \item A/B: For Bs add 0, for As add 4.
#   \item o/c: For shifted (o) add 0, for centered (c) add 2.
#   \item R/F: For antisense (R) add 0, for sense (F) add 1.
#  }
#  Example: (PM,A,c,R) = 8 + 4 + 2 + 0 = 14 (=PMcAR)
# }
#
# @author "HB"
#
# \references{
#  [1] LaFramboise T, Weir BA, Zhao X, Beroukhim R, Li C, Harrington D, 
#      Sellers WR, and Meyerson M. \emph{Allele-specific amplification in 
#      cancer revealed by SNP array analysis}, PLoS Computational Biology,
#      Nov 2005, Volume 1, Issue 6, e65.\cr
# }
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfAddPlasqTypes <- function(groups, ...) {
  nbrOfGroups <- length(groups);

  nbrOfStrands <- nbrOfGroups %/% 2;
  for (kk in 1:nbrOfStrands) {
    groupA <- groups[[2*kk-1]];
    groupB <- groups[[2*kk]];

    # Identify the interrogating probe pair (quartet)
    isInterrogating <- (groupA$tbase != groupB$tbase);
    groupA$isInterrogating <- groupB$isInterrogating <- isInterrogating;

    groups[[2*kk-1]] <- groupA;
    groups[[2*kk]] <- groupB;
  }

  for (gg in seq(along=groups)) {
    group <- groups[[gg]];

    # PM or MM?
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

    # Allele A or B?
    isA <- (gg %% 2 == 1);

    # Sense or antisense?
    fields <- names(group);

    # readCdfUnits() structure?
    pos <- which("direction" == fields);
    if (length(pos) > 0) {
      direction <- .subset2(group, pos);
      isSense <- (direction == 1);
    } else {
      # ...otherwise readCdf().
      direction <- .subset2(group, "groupdirection");
      isSense <- (direction == "sense");
    }

    # Centered or shifted?
    isCentered <- group$isInterrogating;

    #  PM/MM: For MMs add 0, for PMs add 8.
    #  A/B: For Bs add 0, for As add 4.
    #  o/c: For shifted add 0, for centered add 2.
    #  +/-: For antisense add 0, for sense add 1.
    group$plasqType <- (8*isPm + 4*isA + 2*isCentered + 1*isSense);
    
    groups[[gg]] <- group;
  }

  groups;
}

############################################################################
# HISTORY:
# 2007-01-05
# o Made the code work with new readCdfUnits() as well as the readCdf().
#   The former hasn't been tested though.
# 2006-12-30
# o Requires that readCdf() is used.
# o Created.
############################################################################  

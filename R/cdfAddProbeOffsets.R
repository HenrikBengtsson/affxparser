########################################################################/**
# @RdocFunction cdfAddProbeOffsets
#
# @title "Adds probe offsets to the groups in a CDF structure"
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
#    Each group must contain the fields \code{tbase}, and \code{expos}.
#  }
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure with half the number of groups as the 
#  \code{groups} argument (since allele A and allele B groups have
#  been joined).
# }
#
# \seealso{
#  @see "applyCdfGroups".
# }
#
# @author "HB"
#
# \references{
#  [1] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
#      \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
# }
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfAddProbeOffsets <- function(groups, ...) {
  nbrOfGroups <- length(groups);

  nbrOfStrands <- nbrOfGroups %/% 2;
  for (kk in 1:nbrOfStrands) {
    groupA <- groups[[2*kk-1]];
    groupB <- groups[[2*kk]];

    # Identify the interrogating probe pair (quartet)
    isInterrogating <- (groupA$tbase != groupB$tbase);

    # Get the Expos value of the interrogating quartet.
    centerExpos <- which(isInterrogating)[1];

    # Does an interrogating quartet exists?
    hasInterrogating <- !is.na(centerExpos);
    if (hasInterrogating) {
      # Yes: Calculate (true) offsets
      offsetA <- groupA$expos - groupA$expos[centerExpos];
      offsetB <- groupB$expos - groupB$expos[centerExpos];
    } else {
      # No: Guess offsets by assuming first probe pair is shifted
      #     -4 positions.  Assert that no other quartet has zero
      #     shift.  If shift only -3 and so on.
      offsetA <- groupA$expos - groupA$expos[1];
      offsetB <- groupB$expos - groupB$expos[1];
      for (shift in -4:+4) {
        # Results in a zero offset?
        if (any(offsetA != -shift))
          break;
      }
      offsetA <- offsetA + shift;
      offsetB <- offsetB + shift;
    }

    # Add inferred knowledge
    groupA$hasInterrogating <- groupB$hasInterrogating <- hasInterrogating;
    groupA$offset <- offsetA;
    groupB$offset <- offsetB;

    groups[[2*kk-1]] <- groupA;
    groups[[2*kk]] <- groupB;
  }

  groups;
} # cdfAddProbeOffsets()


############################################################################
# HISTORY:
# 2006-06-19
# o Added more Rdoc help.
# 2006-03-07
# o Created.
############################################################################  


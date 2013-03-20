########################################################################/**
# @RdocFunction cdfMergeToQuartets
#
# @title "Function to re-arrange CDF groups values in quartets"
#
# \description{
#  @get "title".
#
#  This @function is design to be used with @see "applyCdfGroups"
#  on an Affymetrix Mapping (SNP) CDF @list structure.
#
#  Note, this requires that the group values have already been 
#  arranged in PMs and MMs.
# }
#
# @synopsis
#
# \arguments{
#  \item{groups}{A @list structure with groups.}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure with the two groups \code{forward}
#  and \code{reverse}, if the latter exists.
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
cdfMergeToQuartets <- function(groups, ...) {
  nbrOfGroups <- length(groups);

  # Allocate the new groups
  nbrOfStrands <- nbrOfGroups %/% 2;
  newGroups <- vector("list", nbrOfStrands);

  for (kk in 1:nbrOfStrands) {
    groupA <- groups[[2*kk-1]];
    groupB <- groups[[2*kk]];

    # Allocate the fields
    nbrOfFields <- length(groupA);
    newGroup <- vector("list", nbrOfFields);

    # Join the fields of allele A and allele B.
    for (ff in seq(length=nbrOfFields)) {
      fieldA <- groupA[[ff]];
      fieldB <- groupB[[ff]];

      ndim <- length(dim(fieldA));
      if (ndim <= 1) {
        # If empty or a vector, stack then into a matrix.
        fieldA <- rbind(fieldA, fieldB);
        rownames(fieldA) <- c("A", "B");
      } else if (ndim == 2) {
        # If a matrix, stack into a new matrix.
        rownames <- c(paste(rownames(fieldA), "A", sep=""), 
                      paste(rownames(fieldB), "B", sep=""));
        fieldA <- rbind(fieldA, fieldB);
        rownames(fieldA) <- rownames;
      } else {
        # Otherwise, just append the values.  Maybe we should
        # do something smarter here?!? /HB 2006-03-07
        fieldA <- append(fieldA, fieldB);
      }
      newGroup[[ff]] <- fieldA;
    }
    names(newGroup) <- names(groupA);
    newGroups[[kk]] <- newGroup;
  }
  names(newGroups) <- rep(c("forward", "reverse"), length.out=nbrOfStrands);
  newGroups;
}


############################################################################
# HISTORY:
# 2006-03-07
# o Renamed from cdfStandJoiner() to cdfMergeStrands().
# 2006-02-23
# o Created.
############################################################################  

########################################################################/**
# @RdocFunction cdfMergeAlleles
#
# @title "Function to join CDF allele A and allele B groups strand by strand"
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
#  \item{groups}{A @list structure with groups.}
#  \item{compReverseBases}{If @TRUE, the group names, which typically are
#    names for bases, are turned into their complementary bases for the
#    reverse strand.}
#  \item{collapse}{The @character string used to collapse the allele A
#    and the allele B group names.}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure with the two groups \code{forward}
#  and \code{reverse}, if the latter exists.
# }
#
# \details{
#  Allele A and allele B are merged into a @matrix where first row
#  hold the elements for allele A and the second elements for allele B.
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
cdfMergeAlleles <- function(groups, compReverseBases=FALSE, collapse="", ...) {
  nbrOfGroups <- length(groups);

  # Allocate the new groups
  nbrOfStrands <- nbrOfGroups %/% 2;
  newGroups <- vector("list", nbrOfStrands);
  groupNames <- names(groups);

  for (kk in 1:nbrOfStrands) {
    kk2 <- 2*kk;
    groupA <- .subset2(groups, kk2-1);
    groupB <- .subset2(groups, kk2);

    # Allocate the fields
    nbrOfFields <- length(groupA);
    newGroup <- vector("list", nbrOfFields);

    # Join the fields of allele A and allele B.
    for (ff in seq(length=nbrOfFields)) {
      fieldA <- .subset2(groupA, ff);
      fieldB <- .subset2(groupB, ff);

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
    } # for (ff ...);
    # Set the name of the fields
    names(newGroup) <- names(groupA);

    newGroups[[kk]] <- newGroup;
  }

  if (compReverseBases) {
    groupNames[c(3,4)] <- c(A="T", C="G", G="C", T="A")[groupNames[c(3,4)]];
  }

  nameFwd <- paste(groupNames[c(1,2)], collapse=collapse);
  if (nbrOfStrands == 2) {
    nameRev <- paste(groupNames[c(3,4)], collapse=collapse);
    names(newGroups) <- c(nameFwd, nameRev);
  } else {
    names(newGroups) <- nameFwd;
  }

  newGroups;
}


############################################################################
# HISTORY:
# 2006-05-04
# o Now the names of the generated groups are constructed from the allele
#   A and B group names.  Before they were only "forward" and "reverse".
# o Renamed from cdfMergeToQuartets().
# 2006-03-07
# o Renamed from cdfStandJoiner() to cdfMergeStrands().
# 2006-02-23
# o Created.
############################################################################  

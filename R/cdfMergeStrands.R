########################################################################/**
# @RdocFunction cdfMergeStrands
#
# @title "Function to join CDF groups with the same names"
#
# \description{
#  @get "title".
#
#  This @function is design to be used with @see "applyCdfGroups"
#  on an Affymetrix Mapping (SNP) CDF @list structure.
#
#  This can be used to join the sense and anti-sense groups of the same 
#  allele in SNP arrays.
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
#  Returns a @list structure with only two groups.
# }
#
# \details{
#  If a unit has two strands, they are merged such that the elements
#  for the second strand are concattenated to the end of the elements
#  of first strand (This is done seperately for the two alleles).
# }
#
# \seealso{
#  @see "applyCdfGroups".
# }
#
# @author
#
# \references{
#  [1] Affymetrix, \emph{Understanding Genotyping Probe Set Structure}, 2005.
#      \url{http://www.affymetrix.com/support/developer/whitepapers/genotyping_probe_set_structure.affx}\cr
# }
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfMergeStrands <- function(groups, ...) {
  nbrOfGroups <- length(groups);
  if (nbrOfGroups != 2 && nbrOfGroups != 4)
    return(groups);

  names <- names(groups);
  unames <- unique(names);

  res <- list();
  for (name in unames) {
    idx <- which(name == names);
    group1 <- groups[[idx[1]]];
    group2 <- groups[[idx[2]]];
    for (kk in seq(along=group1)) {
      fields1 <- group1[[kk]];
      fields2 <- group2[[kk]];
      ndim <- length(dim(fields1));
      if (ndim <= 1) {
        fields1 <- append(fields1, fields2);
      } else if (ndim == 2) {
        fields1 <- cbind(fields1, fields2);
      } else {
        # This should never occur for a normal CDF structure.
        fields1 <- append(fields1, fields2);
      }
      group1[[kk]] <- fields1;
      rm(fields1, fields2);
    }
    res[[name]] <- group1;
    rm(group1, group2, idx);
  }

  res;
} 


############################################################################
# HISTORY:
# 2006-03-07
# o Renamed from cdfStandJoiner() to cdfMergeStrands().
# 2006-02-23
# o Created.
############################################################################  

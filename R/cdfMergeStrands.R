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
cdfMergeStrands <- function(groups, ...) {
  nbrOfGroups <- length(groups);
  if (nbrOfGroups != 2 && nbrOfGroups != 4 && nbrOfGroups %% 2 != 0)
    return(groups);

  names <- names(groups);
  unames <- unique(names);

  res <- list();

  # For each allele...
  for (name in unames) {
    idx <- which(name == names);
    group <- .subset2(groups, idx[1]);
    nfields <- length(group);
    if (nfields > 0) {
      ffs <- 1:nfields;
      idx <- idx[-1];
      while(length(idx) > 0) {
        groupNext <- .subset2(groups, idx[1]);
  
        # For each field...
        for (ff in ffs) {
          fields <- .subset2(group, ff);
          fieldsNext <- .subset2(groupNext, ff);
          ndim <- length(dim(fields));
          if (ndim <= 1) {
            fields <- append(fields, fieldsNext);
          } else if (ndim == 2) {
            fields <- cbind(fields, fieldsNext);
          } else {
            # This should never occur for a normal CDF structure.
            fields <- append(fields, fieldsNext);
          }
          group[[ff]] <- fields;
        }
  
        idx <- idx[-1];
      } # while(...)
    }

    res[[name]] <- group;
  } # for (name ...)

  res;
} # cdfMergeStrands()


############################################################################
# HISTORY:
# 2008-02-22
# o Updated so it now merges any unit with a even number of groups;
#   a custom SNP CDF had three pairs of groups in part of their units.
# 2006-03-07
# o Renamed from cdfStandJoiner() to cdfMergeStrands().
# 2006-02-23
# o Created.
############################################################################  

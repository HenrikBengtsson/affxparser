########################################################################/**
# @RdocFunction cdfGtypeCelToPQ
#
# @title "Function to immitate Affymetrix' gtype\_cel\_to\_pq software"
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
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure with a single group.  The fields in this
#  groups are in turn vectors (all of equal length) where the elements
#  are stored as subsequent quartets (PMA, MMA, PMB, MMB) with all 
#  forward-strand quartets first followed by all reverse-strand quartets.
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
cdfGtypeCelToPQ <- function(groups, ...) {
  nbrOfGroups <- length(groups);
  if (nbrOfGroups < 2)
    return(groups);

  # Merge forward groups 1 & 3, and reverse groups 2 & 4.
  groups <- cdfMergeStrands(groups, ...);

  # Stack allele A and allele B
  groups <- cdfMergeToQuartets(groups, ...);
  
  # Now there is only one group. Rename it to "quartets"
  names(groups) <- "quartets";

  # Vectorize each field
  fields <- groups[[1]];

  fields <- lapply(fields, FUN=as.vector);

  groups[[1]] <- fields;

  groups;
} 


############################################################################
# HISTORY:
# 2006-03-11
# o Created, partly to verify the correctness of affxparser.
############################################################################  

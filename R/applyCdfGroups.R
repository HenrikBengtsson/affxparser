########################################################################/**
# @RdocFunction applyCdfGroups
#
# @title "Applies a function over the groups in a CDF structure"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{cdf}{A CDF @list structure.}
#  \item{fcn}{A @function taking a @list structure of group elements.}
#  \item{...}{Arguments passed to the \code{fcn} function.}
# }
#
# \value{
#  Returns an updated CDF @list structure.
# }
#
# @examples "../incl/applyCdfGroups.Rex"
#
# \seealso{
#   To merge groups with the same name, for instance the forward and
#   reverse strands of SNP arrays, see @see "cdfMergeStrands".
#   To merge fields for allele A and allele B, see
#   @see "cdfMergeToQuartets".
#   To add sequence shifts of the probes, see @see "cdfAddProbeOffsets".
#   To add counts of the number of nucleotides that mismatch the target
#   sequences for allele A and allele B, respectively, see
#   @see "cdfAddBaseMmCounts".
# }
#
# @author
#
# @keyword programming
#**/#######################################################################
applyCdfGroups <- function(cdf, fcn, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'cdf':
  if (!is.list(cdf)) {
    stop("Argument 'cdf' is not a list: ", mode(cdf));
  }

  # Argument 'fcn':
  if (!is.function(fcn)) {
    stop("Argument 'fcn' is not a function: ", mode(fcn));
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Iterate over all unit group sets.
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  for (uu in seq(along=cdf)) {
    unit <- cdf[[uu]];
    unit$groups <- fcn(unit$groups, ...);
    cdf[[uu]] <- unit;
  }

  cdf;
}


############################################################################
# HISTORY:
# 2006-02-23
# o Created.
############################################################################  

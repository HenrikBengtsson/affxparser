########################################################################/**
# @RdocFunction applyCdfGroups
# @alias applyCdfBlocks
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
#  \item{fcn}{A @function that takes a @list structure of group elements
#   and returns an updated @list of groups.}
#  \item{...}{Arguments passed to the \code{fcn} function.}
# }
#
# \value{
#  Returns an updated CDF @list structure.
# }
#
# \section{Pre-defined restructuring functions}{
#  \itemize{
#   \item{Generic:}{
#   \itemize{
#    \item @see "cdfGetFields" - Gets a subset of groups fields in a CDF
#      structure.
#    \item @see "cdfGetGroups" - Gets a subset of groups in a CDF structure.
#    \item @see "cdfOrderBy" - Orders the fields according to the value of 
#      another field in the same CDF group.
#    \item @see "cdfOrderColumnsBy" - Orders the columns of fields according
#      to the values in a certain row of another field in the same CDF group. 
#   }}
#   \item{Designed for SNP arrays:}{
#   \itemize{
#    \item @see "cdfAddBaseMmCounts" - Adds the number of allele A and 
#      allele B mismatching nucleotides of the probes in a CDF structure.
#    \item @see "cdfAddProbeOffsets" - Adds probe offsets to the groups in 
#      a CDF structure.
#    \item @see "cdfGtypeCelToPQ" - Function to immitate Affymetrix' 
#      \code{gtype_cel_to_pq} software.
#    \item @see "cdfMergeAlleles" - Function to join CDF allele A and 
#      allele B groups strand by strand.
#    \item @see "cdfMergeStrands" - Function to join CDF groups with the
#      same names.
#   }}
#  }
#
#  We appreciate contributions.
# }
#
# @examples "../incl/applyCdfGroups.Rex"
#
# @author "HB"
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
    unit <- .subset2(cdf, uu);
    unit$groups <- fcn(.subset2(unit, "groups"), ...);
    cdf[[uu]] <- unit;
  }

  cdf;
}


############################################################################
# HISTORY:
# 2006-12-30
# o Using .subset2() instead of [[().
# 2006-02-23
# o Created.
############################################################################  

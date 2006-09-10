########################################################################/**
# @RdocFunction applyCdfBlocks
#
# @title "Applies a function over the blocks in a CDF structure"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{cdf}{A CDF @list structure.}
#  \item{fcn}{A @function that takes a @list structure of block elements
#   and returns an updated @list of blocks.}
#  \item{...}{Arguments passed to the \code{fcn} function.}
#  \item{.key}{The name of the unit element containing the blocks.  This
#   used for backward compatibility when this field was also called
#   \code{groups}.}
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
#    \item @see "cdfGetFields" - Gets a subset of blocks fields in a CDF
#      structure.
#    \item @see "cdfGetBlocks" - Gets a subset of blocks in a CDF structure.
#    \item @see "cdfOrderBy" - Orders the fields according to the value of 
#      another field in the same CDF block.
#    \item @see "cdfOrderColumnsBy" - Orders the columns of fields according
#      to the values in a certain row of another field in the same CDF block. 
#   }}
#   \item{Designed for SNP arrays:}{
#   \itemize{
#    \item @see "cdfAddBaseMmCounts" - Adds the number of allele A and 
#      allele B mismatching nucleotides of the probes in a CDF structure.
#    \item @see "cdfAddProbeOffsets" - Adds probe offsets to the blocks in 
#      a CDF structure.
#    \item @see "cdfGtypeCelToPQ" - Function to immitate Affymetrix' 
#      \code{gtype_cel_to_pq} software.
#    \item @see "cdfMergeAlleles" - Function to join CDF allele A and 
#      allele B blocks strand by strand.
#    \item @see "cdfMergeStrands" - Function to join CDF blocks with the
#      same names.
#   }}
#  }
#
#  We appreciate contributions.
# }
#
# @author
#
# @keyword programming
#**/#######################################################################
applyCdfBlocks <- function(cdf, fcn, ..., .key="blocks") {
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

  # Nothing to do?
  nbrOfUnits <- length(cdf);
  if (nbrOfUnits == 0)
    return(cdf);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Iterate over all unit block sets.
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Assume that the 'blocks' field have the same position in all units!
  ff <- match(.key, names(cdf[[1]]));
  if (is.na(ff))
    stop("There is no such element in CDF structure: ", .key);

  for (uu in seq(along=cdf)) {
    unit <- .subset2(cdf, uu);
    unit[[ff]] <- fcn(.subset2(unit, ff), ...);
    cdf[[uu]] <- unit;
  }

  cdf;
}


############################################################################
# HISTORY:
# 2006-09-08
# o Created from applyCdfGroups().  This is due to the planned name change 
#   from 'groups' to 'blocks'.
# 2006-02-23
# o Created.
############################################################################  

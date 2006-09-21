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

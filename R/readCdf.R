readCdf <- function(filename, units = NULL, verbose = 0){
  filename <- file.path(dirname(filename), basename(filename))
  if (!file.exists(filename))
    stop("file:", filename, "does not exist.")

  returnUnitType <- TRUE
  returnUnitDirection <- TRUE
  returnUnitAtomNumbers <- TRUE
  returnUnitNumber <- TRUE
  returnXY <- TRUE
  returnIndices <- TRUE
  returnBases <- TRUE
  returnAtoms <- TRUE
  returnIndexpos <- TRUE
  returnBlockDirection <- TRUE
  returnAtomNumbers <- TRUE
  return(.Call("R_affx_get_cdf_file",
               filename, as.integer(units), as.integer(verbose),
               returnUnitType, returnUnitDirection,
               returnUnitAtomNumbers, returnUnitNumber,
               returnXY, returnIndices,
               returnBases, returnAtoms, returnIndexpos,
               returnBlockDirection, returnAtomNumbers,
               PACKAGE="affxparser"))
}

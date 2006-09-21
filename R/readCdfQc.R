readCdfQc <- function(filename, units = NULL, verbose = 0){
  filename <- file.path(dirname(filename), basename(filename))
  if (!file.exists(filename))
    stop("file:", filename, "does not exist.")

  returnIndices <- TRUE;
  returnXY <- TRUE;
  returnLength <- TRUE;
  returnPMInfo <- TRUE;
  returnBackgroundInfo <- TRUE;
  returnType <- TRUE;
  returnQcNumbers <- TRUE;
  return(.Call("R_affx_get_cdf_file_qc",
                filename, as.integer(units), as.integer(verbose),
                returnIndices, returnXY, returnLength, returnPMInfo,
                returnBackgroundInfo, returnType, returnQcNumbers))
}

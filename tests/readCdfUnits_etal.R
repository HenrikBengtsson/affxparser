if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathA <- file.path(pathR, "annotationData", "chipTypes", "Test3")

  # Read CDF structure
  cdf <- file.path(pathA, "1.XDA", "Test3.CDF")
  hdr <- readCdfHeader(cdf)
  Jall <- hdr$nunits

  # Various sets of indices to be read
  idxsList <- list(
##  readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=10L,
    readSome=11:20,
    readDouble=as.double(11:20)
  )

  fcnList <- list(
    readCdf,
    readCdfUnits,
    readCdfUnitNames,
    readCdfNbrOfCellsPerUnitGroup,
    readCdfGroupNames,
    readCdfCellIndices,
    readCdfIsPm
  )

  # Read full file
  for (fcn in fcnList) {
    data <- fcn(cdf)
    str(head(data))
    stopifnot(length(data) == Jall)
  } # for (fcn ...)

  for (fcn in fcnList) {
    # Read different subsets of units
    for (idxs in idxsList) {
      data <- fcn(cdf, units=idxs)
      str(head(data))
      J <- if (is.null(idxs)) Jall else length(idxs)
      stopifnot(length(data) == J)
    }
  } # for (fcn ...)
} # if (require("AffymetrixDataTestFiles"))

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

  # Read full file
  data <- readCdfUnitsWriteMap(cdf)
  str(data)
  Jall <- length(data)

  # Read different subsets of units
  for (idxs in idxsList) {
    data <- readCdfUnitsWriteMap(cdf, units=idxs)
    str(data)
    stopifnot(length(data) == Jall)
  }
} # if (require("AffymetrixDataTestFiles"))

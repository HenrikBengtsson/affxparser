if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathA <- file.path(pathR, "annotationData", "chipTypes", "Test3")
  pathD <- file.path(pathR, "rawData", "FusionSDK_Test3", "Test3")

  # Read CDF structure
  cdf <- file.path(pathA, "1.XDA", "Test3.CDF")
  hdr <- readCdfHeader(cdf)
  Jall <- hdr$nunits

  # Find all CEL files
  cels <- list.files(path=pathD, pattern="[.]CEL$",
                          recursive=TRUE, full.names=TRUE)

  # Various sets of indices to be read
  idxsList <- list(
##  readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=10L,
    readSome=11:20,
    readDouble=as.double(11:20)
  )

  # Read full file
  data <- readCelUnits(cels, cdf=cdf)
  str(head(data))
  stopifnot(length(data) == Jall)

  # Read different subsets of units
  for (idxs in idxsList) {
    data <- readCelUnits(cels, units=idxs, cdf=cdf)
    str(head(data))
    J <- if (is.null(idxs)) Jall else length(idxs)
    stopifnot(length(data) == J)
  }
} # if (require("AffymetrixDataTestFiles"))

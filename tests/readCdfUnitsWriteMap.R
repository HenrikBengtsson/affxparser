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
    readDouble=as.double(11:20),
    outOfRange=-1L,
    outOfRange=0L,
    outOfRange=1e9L
  )

  # Read full file
  data <- readCdfUnitsWriteMap(cdf)
  str(data)
  Jall <- length(data)

  # Read different subsets of units
  for (ii in seq_along(idxsList)) {
    name <- names(idxsList)[ii]
    message(sprintf("Testing readCdfUnitsWriteMap() with '%s' indices...", name))
    idxs <- idxsList[[ii]]
    str(list(idxs=idxs))
    if (grepl("outOfRange", name)) {
      res <- tryCatch(readCdfUnitsWriteMap(cdf, units=idxs), error=function(ex) ex)
      str(res)
      stopifnot(inherits(res, "error"))
    } else {
      data <- readCdfUnitsWriteMap(cdf, units=idxs)
      str(data)
      stopifnot(length(data) == Jall)
    }
    message(sprintf("Testing readCdfUnitsWriteMap() with '%s' indices...done", name))
  } # for (ii ...)
} # if (require("AffymetrixDataTestFiles"))

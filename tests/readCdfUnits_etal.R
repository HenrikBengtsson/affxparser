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

  fcnNames <- c(
    "readCdf",
    "readCdfUnits",
    "readCdfUnitNames",
    "readCdfNbrOfCellsPerUnitGroup",
    "readCdfGroupNames",
    "readCdfCellIndices",
    "readCdfIsPm"
  )

  # Read full file
  for (fcnName in fcnNames) {
    fcn <- get(fcnName, mode="function", envir=getNamespace("affxparser"))
    data <- fcn(cdf)
    str(head(data))
    stopifnot(length(data) == Jall)
  } # for (fcn ...)

  for (fcnName in fcnNames) {
    fcn <- get(fcnName, mode="function", envir=getNamespace("affxparser"))

    # Read different subsets of units
    for (ii in seq_along(idxsList)) {
      name <- names(idxsList)[ii]
      message(sprintf("Testing %s() with '%s' indices...", fcnName, name))
      idxs <- idxsList[[ii]]
      str(list(idxs=idxs))
      if (grepl("outOfRange", name)) {
        res <- tryCatch(readCdfQc(cdf, units=idxs), error=function(ex) ex)
        str(res)
        stopifnot(inherits(res, "error"))
      } else {
        data <- fcn(cdf, units=idxs)
        str(head(data))
        J <- if (is.null(idxs)) Jall else length(idxs)
        stopifnot(length(data) == J)
      }
      message(sprintf("Testing %s() with '%s' indices...done", fcnName, name))
    } # for (ii ...)
  } # for (fcn ...)
} # if (require("AffymetrixDataTestFiles"))

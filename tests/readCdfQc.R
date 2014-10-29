if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathA <- file.path(pathR, "annotationData", "chipTypes", "Test3")

  # Read CDF structure
  cdf <- file.path(pathA, "1.XDA", "Test3.CDF")
  hdr <- readCdfHeader(cdf)
  Jall <- hdr$nqcunits

  # Various sets of indices to be read
  idxsList <- list(
##  readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=5L,
    readSome=5:10,
    readDouble=as.double(5:10),
    outOfRange=-1L,
    outOfRange=0L,
    outOfRange=1e9L
  )

  # Read full file
  data <- readCdfQc(cdf)
  str(head(data))
  stopifnot(length(data) == Jall)

  # Read different subsets of units
  for (ii in seq_along(idxsList)) {
    name <- names(idxsList)[ii]
    message(sprintf("Testing readCdfQc() with '%s' indices...", name))
    idxs <- idxsList[[ii]]
    str(list(idxs=idxs))
    if (grepl("outOfRange", name)) {
      res <- tryCatch(readCdfQc(cdf, units=idxs), error=function(ex) ex)
      str(res)
      stopifnot(inherits(res, "error"))
    } else {
      data <- readCdfQc(cdf, units=idxs)
      str(head(data))
      J <- if (is.null(idxs)) Jall else length(idxs)
      stopifnot(length(data) == J)
    }
    message(sprintf("Testing readCdfQc() with '%s' indices...done", name))
  } # for (ii ...)
} # if (require("AffymetrixDataTestFiles"))

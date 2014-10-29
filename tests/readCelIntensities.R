if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathD <- file.path(pathR, "rawData", "FusionSDK_Test3", "Test3")

  # Find all CEL files
  path <- file.path(pathD, "2.Calvin")
  cels <- list.files(path=path, pattern="[.]CEL$", full.names=TRUE)

  hdr <- readCelHeader(cels[1L])
  I <- length(cels)
  Jall <- hdr$total

  # Read full file
  data <- readCelIntensities(cels)
  str(data)
  stopifnot(all(dim(data) == c(Jall,I)))

  # Various sets of indices to be read
  idxsList <- list(
#  readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=10L,
    readSome=11:20,
    readDouble=as.double(11:20),
    outOfRange=-1L,
    outOfRange=0L,
    outOfRange=1e9L
  )

  # Read different subsets of cells
  for (ii in seq_along(idxsList)) {
    name <- names(idxsList)[ii]
    message(sprintf("Testing readCelIntensities() with '%s' indices...", name))
    idxs <- idxsList[[ii]]
    str(list(idxs=idxs))
    if (grepl("outOfRange", name)) {
      res <- tryCatch(readCelIntensities(cels, indices=idxs), error=function(ex) ex)
      str(res)
      stopifnot(inherits(res, "error"))
    } else {
      data <- readCelIntensities(cels, indices=idxs)
      str(data)
      J <- if (is.null(idxs)) Jall else length(idxs)
      stopifnot(all(dim(data) == c(J,I)))
    }
    message(sprintf("Testing readCelIntensities() with '%s' indices...done", name))
  } # for (ii ...)
} # if (require("AffymetrixDataTestFiles"))

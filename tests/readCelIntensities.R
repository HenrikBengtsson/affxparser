if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  path <- file.path(pathR, "rawData", "FusionSDK_Test3", "Test3")

  # Find all CEL files
  path <- file.path(path, "2.Calvin")
  pathnames <- list.files(path=path, pattern="[.]CEL$", full.names=TRUE)

  hdr <- readCelHeader(pathnames[1L])
  I <- length(pathnames)
  Jall <- hdr$total

  # Read full file
  data <- readCelIntensities(pathnames)
  str(data)
  stopifnot(all(dim(data) == c(Jall,I)))

  # Various sets of indices to be read
  idxsList <- list(
##  readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=10L,
    readSome=11:20,
    readDouble=as.double(11:20)
  )

  # Read different subsets of the file
  for (idxs in idxsList) {
    data <- readCelIntensities(pathnames, indices=idxs)
    str(data)
    J <- if (is.null(idxs)) Jall else length(idxs)
    stopifnot(all(dim(data) == c(J,I)))
  }
} # if (require("AffymetrixDataTestFiles"))

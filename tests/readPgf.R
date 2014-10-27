if (require("AffymetrixDataTestFiles") && packageVersion("AffymetrixDataTestFiles") >= "0.4.0") {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathA <- file.path(pathR, "annotationData", "chipTypes", "HuGene-1_0-st-v1")

  # Read PGF structure
  pgf <- file.path(pathA, "HuGene-1_0-st-v1.r4,10_probesets.pgf")

  # Various sets of indices to be read
  idxsList <- list(
##  readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=5L,
    readSome=1:5,
    readDouble=as.double(1:5) # FIX ME
  )

  data <- readPgf(pgf)
  str(head(data))
  stopifnot(identical(data$header$chip_type, "HuGene-1_0-st-v1"))
  Jall <- length(data$probesetName)

  # Read different subsets of units
  for (idxs in idxsList) {
    data <- readPgf(pgf, indices=idxs)
    str(head(data))
    stopifnot(identical(data$header$chip_type, "HuGene-1_0-st-v1"))
    J <- if (is.null(idxs)) Jall else length(idxs)
    stopifnot(length(data$probesetName) == J)
  }
} # if (require("AffymetrixDataTestFiles"))

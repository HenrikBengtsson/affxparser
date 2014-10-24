if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathD <- file.path(pathR, "rawData", "FusionSDK_Test3", "Test3")

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

  for (kk in seq_along(cels)) {
    cel <- cels[kk]

    # Read full file
    data <- readCel(cel)
    str(data)
    Jall <- data$header$total
    stopifnot(length(data$intensities) == Jall)

    # Read different subsets of cells
    for (idxs in idxsList) {
      data <- readCel(cel, indices=idxs)
      str(data)
      J <- if (is.null(idxs)) Jall else length(idxs)
      stopifnot(length(data$intensities) == J)
    }
  } # for (kk ...)
} # if (require("AffymetrixDataTestFiles"))

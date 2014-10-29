if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathD <- file.path(pathR, "rawData", "FusionSDK_Test3", "Test3")

  # Find all CEL files
  cels <- list.files(path=pathD, pattern="[.]CEL$",
                     recursive=TRUE, full.names=TRUE)

  # Various sets of indices to be read
  idxsList <- list(
#    readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=10L,
    readSome=11:20,
    readDouble=as.double(11:20),
    outOfRange=-1L,
    outOfRange=0L,
    outOfRange=1e9L
  )

  for (kk in seq_along(cels)) {
    cel <- cels[kk]

    # Read full file
    data <- readCel(cel)
    str(data)
    Jall <- data$header$total
    stopifnot(length(data$intensities) == Jall)

    # Read different subsets of cells
    for (ii in seq_along(idxsList)) {
      name <- names(idxsList)[ii]
      message(sprintf("Testing readCel() with '%s' indices...", name))
      idxs <- idxsList[[ii]]
      str(list(idxs=idxs))
      if (grepl("outOfRange", name)) {
        res <- tryCatch(readCel(cel, indices=idxs), error=function(ex) ex)
        str(res)
        stopifnot(inherits(res, "error"))
      } else {
        data <- readCel(cel, indices=idxs)
        str(data)
        J <- if (is.null(idxs)) Jall else length(idxs)
        stopifnot(length(data$intensities) == J)
      }
      message(sprintf("Testing readCel() with '%s' indices...done", name))
    } # for (ii ...)
  } # for (kk ...)
} # if (require("AffymetrixDataTestFiles"))

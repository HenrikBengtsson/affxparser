if (require("AffymetrixDataTestFiles") && packageVersion("AffymetrixDataTestFiles") >= "0.4.0") {
  library("affxparser")

  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathA <- file.path(pathR, "annotationData", "chipTypes", "HuGene-1_0-st-v1")

  # Read PGF structure
  pgf <- file.path(pathA, "HuGene-1_0-st-v1.r4,10_probesets.pgf")

  # NOTE: Hard-coded
  Jall <- 10L

  # Various sets of indices to be read
  idxsList <- list(
##    readNothing=integer(0L), # FIX ME
    readAll=NULL,
    readOne=5L,
    readSome=1:5,
    readDouble=as.double(1:5),
    outOfRange=-1L,
    outOfRange=0L,
    outOfRange=1e9L
  )

  data <- readPgf(pgf)
  str(head(data))
  stopifnot(identical(data$header$chip_type, "HuGene-1_0-st-v1"))
  stopifnot(length(data$probesetName) == Jall)

  # Read different subsets of units
  for (ii in seq_along(idxsList)) {
    name <- names(idxsList)[ii]
    message(sprintf("Testing readPgf() with '%s' indices...", name))
    idxs <- idxsList[[ii]]
    str(list(idxs=idxs))
    if (grepl("outOfRange", name)) {
      res <- tryCatch(readPgf(pgf, indices=idxs), error=function(ex) ex)
      str(res)
      stopifnot(inherits(res, "error"))
    } else {
      data <- readPgf(pgf, indices=idxs)
      str(head(data))
      stopifnot(identical(data$header$chip_type, "HuGene-1_0-st-v1"))
      J <- if (is.null(idxs)) Jall else length(idxs)
      stopifnot(length(data$probesetName) == J)
    }
    message(sprintf("Testing readPgf() with '%s' indices...done", name))
  } # for (ii ...)


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate correctness of subsets
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  subsetPgf <- function(data, indices=NULL, ...) {
    if (is.null(indices)) return(data)

    # Atoms
    offsets <- data$probesetStartAtom
    natoms <- diff(c(offsets, length(data0$atomStartProbe)+1L))
    offsets <- offsets[indices]
    natoms <- natoms[indices]
    # Identify atoms to keep
    keep <- logical(length(data$atomStartProbe))
    for (kk in seq_along(offsets)) {
      keep[seq(from=offsets[kk], by=1L, length=natoms[kk])] <- TRUE;
    }

    for (ff in c("probeSequence", "probeId", "probeGcCount", "atomExonPosition", "atomId", "probeInterrogationPosition", "probeLength", "probeType")) {
      data[[ff]] <- data[[ff]][keep]
    }

    data$atomStartProbe <- seq_len(sum(natoms))
    data$probesetStartAtom <- c(1L, cumsum(natoms))[length(indices)]

    # Probesets
    for (ff in c("probesetName", "probesetId", "probesetType")) {
      data[[ff]] <- data[[ff]][indices]
    }

    data
  } # subsetPgf()

  data0 <- readPgf(pgf)
  Jall <- length(data0$probesetId)

  for (kk in 1:10) {
    n <- sample(Jall, size=1L)
    idxs <- sort(sample(1:Jall, size=n, replace=FALSE))
    data <- readPgf(pgf, indices=idxs)
    dataS <- subsetPgf(data0, indices=idxs)
    for (ff in c("probesetStartAtom", "atomExonPosition"))
      data[[ff]] <- dataS[[ff]] <- NULL
    stopifnot(all.equal(data, dataS))
  }
} # if (require("AffymetrixDataTestFiles"))

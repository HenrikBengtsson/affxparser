if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  ## rawData/
  pathR <- system.file(package="AffymetrixDataTestFiles", mustWork=TRUE)
  pathR <- file.path(pathR, "rawData")

  ## File #1: Test3
  chipType <- "Test3"
  path <- file.path(pathR, "FusionSDK_Test3", chipType, "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  hdr <- readCelHeader(pathname)
  str(hdr)
  stopifnot(hdr$chiptype == chipType)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, verbose=TRUE, .validate=TRUE)
  print(pathname4)
  hdr4 <- readCelHeader(pathname4)
  str(hdr4)
  stopifnot(hdr4$chiptype == hdr$chiptype)

  ## New chip type
  newChipType <- sprintf("%s-custom", chipType)
  filename4 <- gsub(".CEL", ",v4,custom.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, newChipType=newChipType, verbose=TRUE)
  print(pathname4)
  hdr4 <- readCelHeader(pathname4)
  str(hdr4)
## FIXME
## stopifnot(hdr4$chiptype == newChipType)


  ## File #2: FusionSDK_HG-U133A
  chipType <- "HG-U133A"
  path <- file.path(pathR, "FusionSDK_HG-U133A", chipType, "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  hdr <- readCelHeader(pathname)
  str(hdr)
  stopifnot(hdr$chiptype == chipType)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, verbose=TRUE, .validate=TRUE)
  print(pathname4)
  hdr4 <- readCelHeader(pathname4)
  str(hdr4)
  stopifnot(hdr4$chiptype == hdr$chiptype)

  ## New chip type
  newChipType <- sprintf("%s-custom", chipType)
  filename4 <- gsub(".CEL", ",v4,custom.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, newChipType=newChipType, verbose=TRUE)
  print(pathname4)
  hdr4 <- readCelHeader(pathname4)
  str(hdr4)
  stopifnot(hdr4$chiptype == newChipType)


  ## File #3: FusionSDK_Focus
  chipType <- "HG-Focus"
  path <- file.path(pathR, "FusionSDK_HG-Focus", chipType, "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  hdr <- readCelHeader(pathname)
  str(hdr)
  stopifnot(hdr$chiptype == chipType)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, verbose=TRUE, .validate=TRUE)
  print(pathname4)
  hdr4 <- readCelHeader(pathname4)
  str(hdr4)
  stopifnot(hdr4$chiptype == hdr$chiptype)

  ## New chip type
  newChipType <- sprintf("%s-custom", chipType)
  filename4 <- gsub(".CEL", ",v4,custom.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, newChipType=newChipType, verbose=TRUE)
  print(pathname4)
  hdr4 <- readCelHeader(pathname4)
  str(hdr4)
##  FIXME
##  stopifnot(hdr4$chiptype == newChipType)
} # if (require("AffymetrixDataTestFiles"))

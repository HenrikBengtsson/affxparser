if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  ## rawData/
  pathR <- system.file(package="AffymetrixDataTestFiles", mustWork=TRUE)
  pathR <- file.path(pathR, "rawData")

  ## File #1: Test3
  path <- file.path(pathR, "FusionSDK_Test3", "Test3", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4)
  print(pathname4)

  ## File #2: FusionSDK_HG-U133A
  path <- file.path(pathR, "FusionSDK_HG-U133A", "HG-U133A", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4)
  print(pathname4)

  ## File #3: FusionSDK_Focus
  path <- file.path(pathR, "FusionSDK_HG-Focus", "HG-Focus", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4)
  print(pathname4)
} # if (require("AffymetrixDataTestFiles"))

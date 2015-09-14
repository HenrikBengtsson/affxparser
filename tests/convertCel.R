if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  ## rawData/
  pathR <- system.file(package="AffymetrixDataTestFiles", mustWork=TRUE)
  pathR <- file.path(pathR, "rawData")

  ## File #1a: Test3
  path <- file.path(pathR, "FusionSDK_Test3", "Test3", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, verbose=TRUE, .validate=TRUE)
  print(pathname4)

  ## File #1b: Test3 with updated chip type
  path <- file.path(pathR, "FusionSDK_Test3", "Test3", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4,custom.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, newChipType="Test3-custom", verbose=TRUE)
  print(pathname4)

  ## File #2a: FusionSDK_HG-U133A
  path <- file.path(pathR, "FusionSDK_HG-U133A", "HG-U133A", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, verbose=TRUE, .validate=TRUE)
  print(pathname4)

  ## File #2b: FusionSDK_HG-U133A with new chip type
  path <- file.path(pathR, "FusionSDK_HG-U133A", "HG-U133A", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4,custom.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, newChipType="HG-U133A-custom", verbose=TRUE)
  print(pathname4)

  ## File #3b: FusionSDK_Focus
  path <- file.path(pathR, "FusionSDK_HG-Focus", "HG-Focus", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, verbose=TRUE, .validate=TRUE)
  print(pathname4)

  ## File #3b: FusionSDK_Focus
  path <- file.path(pathR, "FusionSDK_HG-Focus", "HG-Focus", "2.Calvin")
  filename <- list.files(path=path, pattern="[.]CEL$")[1]
  pathname <- file.path(path, filename)
  filename4 <- gsub(".CEL", ",v4,custom.CEL", filename)
  pathname4 <- convertCel(pathname, filename4, newChipType="HG-U133A-custom", verbose=TRUE)
  print(pathname4)
} # if (require("AffymetrixDataTestFiles"))

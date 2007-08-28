#############################################################################
# This script will test:
#  readCelHeader()
#  readCel()
#############################################################################
library("affxparser");
library("AffymetrixDataTestFiles");
library("R.utils");  # filePath()

logMemory <- exists("memory.size", mode="function");
memSizeRange <- NA;


# Search for some available Calvin CEL files
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE, firstOnly=FALSE)
files <- grep("FusionSDK_Test3", files, value=TRUE)
files <- grep("Calvin", files, value=TRUE)
celFiles <- files
nbrOfFiles <- length(celFiles);
if (nbrOfFiles == 0)
  stop("No CEL files found");
cat(sprintf("Found %d CEL file(s)\n", nbrOfFiles));


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Start torturing
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
settings <- getOption("affxparser.settings");
tortureCount <- settings$tests$torture;
if (is.null(tortureCount))
  tortureCount <- 25
for (kk in 1:tortureCount) {
  cat(sprintf("Round %d of %d:\n", kk, tortureCount));

  # Sample a CEL file
  celFile <- celFiles[sample(nbrOfFiles, size=1)];
  header <- readCelHeader(celFile);
  maxNbrOfCells <- header$rows * header$cols;

  # Sample cells to read
  nbrOfCells <- as.integer(runif(n=1, min=0, max=maxNbrOfCells-0.5));
  cells <- sample(1:maxNbrOfCells, size=nbrOfCells);
  cat(sprintf("Reading %d random cells (in [%d,%d]) in random order from %s.\n", nbrOfCells, min(cells), max(cells), celFile));

  # Sample 'readXY', 'readPixels' and 'readStdvs'.
  readXY <- sample(c(TRUE,FALSE), size=1);
  readIntensities <- sample(c(TRUE,FALSE), size=1);
  readStdvs <- sample(c(TRUE,FALSE), size=1);
  readPixels <- sample(c(TRUE,FALSE), size=1);
  cat(sprintf("Reading (x,y): %s\n", as.character(readXY)));
  cat(sprintf("Reading intensities: %s\n", as.character(readIntensities)));
  cat(sprintf("Reading stdvs: %s\n", as.character(readStdvs)));
  cat(sprintf("Reading pixels: %s\n", as.character(readPixels)));

  # Reading CEL elements
  t <- system.time({
  cel <- readCel(celFile, indices=cells, readXY=readXY, readIntensities=readIntensities, readStdvs=readStdvs, readPixels=readPixels);
  }, gcFirst=FALSE)[3]

  cat(sprintf("Number of cells read: %d\n", length(cel)));
  cat(sprintf("Read time: %.2fs = %.3fs/1000 unit\n", t, 1000*t/length(cel)));
  celSize <- object.size(cel);
  cat(sprintf("Size: %.3gMB = %.2f bytes/probe\n", celSize/1024^2, celSize/length(cel)));

  if (logMemory) {
    memSize <- memory.size();
    memSizeRange <- range(memSizeRange, memSize, na.rm=TRUE);
    cat(sprintf("Memory size: %.2fMB in [%.2fMB, %.2fMB]\n", 
            memSize/1024^2, memSizeRange[1]/1024^2, memSizeRange[2]/1024^2));
  }
  cat("\n");

  rm(cel);
}

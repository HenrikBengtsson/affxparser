#############################################################################
# This script will test:
#  readCelHeader()
#  readCdfHeader()
#  readCelUnits()
# and indirectly:
#  readCdfCellIndices() and readCel()
#############################################################################
library("affxparser");
library("AffymetrixDataTestFiles");
library("R.utils");  # filePath()

set.seed(1);
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
  chipType <- readCelHeader(celFile)$chiptype;
  cdfFile <- findCdf(chipType);
  maxNbrOfUnits <- readCdfHeader(cdfFile)$probesets;

  # Sample units to read
  nbrOfUnits <- as.integer(runif(n=1, min=0, max=maxNbrOfUnits-0.5));
  nbrOfUnits <- as.integer(nbrOfUnits / 4 + 1);
  units <- sample(1:maxNbrOfUnits, size=nbrOfUnits);
  cat(sprintf("Reading %d random units (in [%d,%d]) in random order.\n", 
                                   nbrOfUnits, min(units), max(units)));

  # Sample 'readXY', 'readPixels' 'readStdvs', and 'stratifyBy'.
  readXY <- sample(c(TRUE,FALSE), size=1);
  readIntensities <- sample(c(TRUE,FALSE), size=1);
  readStdvs <- sample(c(TRUE,FALSE), size=1);
  readPixels <- sample(c(TRUE,FALSE), size=1);
  stratifyBy <- c("nothing", "pmmm", "pm", "mm")[sample(4,size=1)];
  readXY <- TRUE;
  readIntensities <- TRUE;
  readStdvs <- TRUE;
  readPixels <- TRUE;
  stratifyBy <- "pmmm";
  stratifyBy <- "nothing";
  cat(sprintf("Reading (x,y): %s\n", as.character(readXY)));
  cat(sprintf("Reading intensities: %s\n", as.character(readIntensities)));
  cat(sprintf("Reading stdvs: %s\n", as.character(readStdvs)));
  cat(sprintf("Reading pixels: %s\n", as.character(readPixels)));
  cat(sprintf("Stratifying by: %s\n", stratifyBy));

  # Reading CEL elements
#  gc();
  t <- system.time({
  res <- readCelUnits(celFile, units=units, readXY=readXY, readIntensities=readIntensities, readStdvs=readStdvs, readPixels=readPixels, stratifyBy=stratifyBy, verbose=TRUE);
  }, gcFirst=FALSE)[3]

  str(res[1]);

  cat(sprintf("Number of units read: %d\n", length(res)));
  cat(sprintf("Read time: %.2fs = %.3fs/1000 unit\n", t, 1000*t/length(res)));
  resSize <- object.size(res);
  cat(sprintf("Size: %.3gMB = %.2f bytes/unit\n", resSize/1024^2, resSize/length(res)));

  if (logMemory) {
    memSize <- memory.size();
    memSizeRange <- range(memSizeRange, memSize, na.rm=TRUE);
    cat(sprintf("Memory size: %.2fMB in [%.2fMB, %.2fMB]\n", 
            memSize/1024^2, memSizeRange[1]/1024^2, memSizeRange[2]/1024^2));
  }
  cat("\n");

  rm(res);
}

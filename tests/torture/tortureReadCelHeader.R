#############################################################################
# This script will test:
#  readCelHeader()
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

  # Reading CEL header
  t <- system.time({
  header <- readCelHeader(celFile);
  }, gcFirst=FALSE)[3]

  cat(sprintf("Number header elements read: %d\n", length(header)));
  cat(sprintf("Read time: %.2fms\n", 1000*t));
  size <- object.size(header);
  cat(sprintf("Size: %.3gkB\n", size/1024));

  if (logMemory) {
    memSize <- memory.size();
    memSizeRange <- range(memSizeRange, memSize, na.rm=TRUE);
    cat(sprintf("Memory size: %.2fMB in [%.2fMB, %.2fMB]\n", 
            memSize/1024^2, memSizeRange[1]/1024^2, memSizeRange[2]/1024^2));
  }
  cat("\n");
}

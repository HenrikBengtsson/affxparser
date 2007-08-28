#############################################################################
# This script will test:
#  readCdfHeader()
#  readCdfUnits()
#############################################################################
library("affxparser");
library("AffymetrixDataTestFiles");

set.seed(1);
logMemory <- exists("memory.size", mode="function");
memSizeRange <- NA;


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Search for a CDF file
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdfFile <- findCdf("Mapping10K_Xba131");
if (is.null(cdfFile))
  stop("No CDF file found");

cat("Found CDF file: ", cdfFile, "\n", sep="")


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Start torturing
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
settings <- getOption("affxparser.settings");
tortureCount <- settings$tests$torture;
if (is.null(tortureCount))
  tortureCount <- 25
for (kk in 1:tortureCount) {
  cat(sprintf("Round %d of %d:\n", kk, tortureCount));

  # Sample units to read
  maxNbrOfUnits <- readCdfHeader(cdfFile)$probesets;
  nbrOfUnits <- as.integer(runif(n=1, min=0, max=maxNbrOfUnits-0.5));
  nbrOfUnits <- as.integer(nbrOfUnits / 4 + 1);
  units <- sample(maxNbrOfUnits, size=nbrOfUnits);
  cat(sprintf("Reading %d random units (in [%d,%d]) in random order.\n", 
                                   nbrOfUnits, min(units), max(units)));

  # Sample 'stratifyBy'
  stratifyBy <- c("nothing", "pmmm", "pm", "mm")[sample(4,size=1)];
  cat(sprintf("Stratifying by: %s.\n", stratifyBy));

  # Reading CDF units
  t <- system.time({
  res <- readCdfUnits(cdfFile, units=units, stratifyBy=stratifyBy, verbose=1);
  }, gcFirst=FALSE)[3]

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

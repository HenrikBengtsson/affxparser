##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Search for some available CEL files
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE, firstOnly=FALSE)
files <- grep("FusionSDK_Test3", files, value=TRUE)
files <- grep("Calvin", files, value=TRUE)

# Fake more CEL files if not enough
files <- rep(files, length.out=5)


print(files);

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Benchmarking reading cells in order or not.
#
# The difference will be large the more files that are read.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
units <- 56:256
nunits <- length(units);

nfiles <- length(files);

t1 <- system.time({
  cel <- readCelUnits(files, units=units, reorder=TRUE)
})[3]
cat(sprintf("Time   [ordered]: %6.2fs = %.2fms/(unit & array) [1.00x]\n", 
                                          t1, 1000*t1/nunits/nfiles))
rm(cel); gc()

t2 <- system.time({
  cel <- readCelUnits(files, units=units, reorder=FALSE)
})[3]
cat(sprintf("Time [unordered]: %6.2fs = %.2fms/(unit & array) [%.2fx]\n", 
                                   t2, 1000*t2/nunits/nfiles, t2/t1))
rm(cel); gc()


# Clean up
rm(files, t1, t2)


##############################################################
}                                                     # STOP #
##############################################################

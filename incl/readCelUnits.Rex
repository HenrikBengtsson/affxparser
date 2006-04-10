
for (zzz in 0) {

# Scan current directory for CEL files
files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
files <- files[!file.info(files)$isdir]
if (length(files) == 0)
  break

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Benchmarking reading cells in order or not.
#
# The difference will be large the more files that are read.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
units <- 56:256
nunits <- length(units);

# Make sure enough files are read to measure the speed up
files <- rep(files, length.out=5)
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

} # for (zzz in 0)

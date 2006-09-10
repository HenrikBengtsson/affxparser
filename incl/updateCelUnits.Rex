for (zzz in 0) {

# CEL file to use in this demostration
pathname <- "tmp.cel"
if (!file.exists(pathname)) {
  # Scan current directory for a CEL file
  file <- findFiles(pattern="[.](c|C)(e|E)(l|L)$")
  if (is.null(file))
    break

  # Work on a copy of the CEL file
  cat("Copying CEL file: ", file, "\n", sep="");
  if (!file.copy(file, pathname))
    stop("Failed to copy CEL file: ", file);
  rm(file)
}


# Check for the CDF file
hdr <- readCelHeader(pathname)
cdfFile <- findCdf(hdr$chiptype)
if (is.null(cdfFile))
  break

hdr <- readCdfHeader(cdfFile)
nbrOfUnits <- hdr$nunits
print(nbrOfUnits);

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Example: Read and re-write the same data
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
units <- c(101, 51)
data1 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
cat("Original data:\n")
str(data1)
updateCelUnits(pathname, data=data1)
data2 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
cat("Updated data:\n")
str(data2)
stopifnot(identical(data1, data2))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Example: Random read and re-write "stress test"
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
for (kk in 1:10) {
  nunits <- sample(min(1000,nbrOfUnits), size=1)
  units <- sample(nbrOfUnits, size=nunits)
  cat(sprintf("%02d. Selected %d random units: reading", kk, nunits));
  t <- system.time({
    data1 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
  }, gcFirst=TRUE)[3]
  cat(sprintf(" [%.2fs=%.2fs/unit], updating", t, t/nunits))
  t <- system.time({
    updateCelUnits(pathname, data=data1)
  }, gcFirst=TRUE)[3]
  cat(sprintf(" [%.2fs=%.2fs/unit], validating", t, t/nunits))
  data2 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
  stopifnot(identical(data1, data2))
  cat(". done\n")
}

} # for (zzz in 0)

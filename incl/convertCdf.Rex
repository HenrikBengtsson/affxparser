for (zzz in 0) {

# Find the small 'Test3' CDF file
cdfFile <- findCdf("Test3")
# Find any CDF file
if (is.null(cdfFile))
  cdfFile <- findCdf()
if (is.null(cdfFile))
  break

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Create an identical CDF structure
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdfHeader <- readCdfHeader(cdfFile)
outFile <- sprintf("%s-copy.cdf", cdfHeader$chiptype)
convertCdf(cdfFile, outFile, overwrite=TRUE, verbose=TRUE)

# Clean up
rm(cdfFile, pathname, cdfHeader)

} # for (zzz in 0)

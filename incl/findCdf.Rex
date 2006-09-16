
# Find a specific CDF file
cdfFile <- findCdf("Mapping50K_Xba240")
print(cdfFile)

# Find the first CDF file (no matter what it is)
cdfFile <- findCdf()
print(cdfFile)

# Find all CDF files in search path and display their headers
cdfFiles <- findCdf(firstOnly=FALSE)
for (cdfFile in cdfFiles) {
  cat("=======================================\n")
  hdr <- readCdfHeader(cdfFile)
  str(hdr)
}

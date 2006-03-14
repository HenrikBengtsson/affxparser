# Find a specific CDF file
cdfFile <- findCdf("Mapping50K_Xba240")
print(cdfFile)

# Find all CDF files in search path
cdfFiles <- findCdf(firstOnly=FALSE)
print(cdfFiles)

# Find the first CDF file (no matter what it is)
cdfFile <- findCdf()
print(cdfFile)


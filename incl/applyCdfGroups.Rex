
for (zzz in 0) {

# Find the CDF file for the Mapping 100K Xba array
chipType <- "Mapping50K_Xba240";
cdfFile <- findCdf(chipType)
if (is.null(cdfFile))
  break;

# Probeset SNP_A-1658298 (dbSNP rs7034018) has equal number of
# sense and anti-sense probes, but for the latter group the
# SNP "interrogating" probe pair has been excluded in
# Affymetrix' probe selection process when designing the
# Mapping 100K Xba array.

# Identify the unit index from the unit name
probeset <- "SNP_A-1658298"
unit <- which(readCdfUnitNames(cdfFile) == probeset)
rm(probeset)

# Read the CDF file
cdf0 <- readCdfUnits(cdfFile, units=unit, stratifyBy="pmmm")
cat("Default CDF structure:\n")
print(cdf0)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Infer the (true or the relative) offset for probe quartets.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdf <- applyCdfGroups(cdf0, cdfAddProbeOffsets)
cat("Probe offsets:\n")
print(cdf)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Identify the number of nucleotides that mismatch the 
# allele A and the allele B sequences, respectively.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdf <- applyCdfGroups(cdf, cdfAddBaseMmCounts)
cat("Allele A & B target sequence mismatch counts:\n")
print(cdf)



# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Combine the signals from  the sense and the anti-sense 
# strands in a SNP CEL files.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# First, join the strands in the CDF structure. 
cdf <- applyCdfGroups(cdf, cdfMergeStrands)
cat("Joined CDF structure:\n")
print(cdf)

# Then, read the CEL file using this new structure
# celFile <- "NA06985_Hind_B5_3005533.CEL"
# cel <- readCelUnits(celFile, cdf=cdf)
# cat("Joined CEL intensities:\n")
# print(cel)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Rearrange values of group fields into quartets.  This
# requires that the values are already arranged as PMs and MMs.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdf <- applyCdfGroups(cdf0, cdfMergeAlleles)
cat("Probe quartets:\n")
print(cdf)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Get the x and y cell locations
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
x <- unlist(applyCdfGroups(cdf, cdfGetFields, "x"), use.names=FALSE)
y <- unlist(applyCdfGroups(cdf, cdfGetFields, "y"), use.names=FALSE)

# Clean up
rm(chipType, cdfFile, unit, cdf0, cdf, x, y)
} # for (zzz in 0)
rm(zzz)


for (zzz in 0) {  # Only so that 'break' can be used

# Scan current directory for CEL files
outFile <- "zzz.CEL"
files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
files <- setdiff(files, outFile)
if (length(files) == 0)
  break

file <- files[1]


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Read the CEL header
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
hdr <- readCelHeader(file)
if (hdr$version != 4)
  break

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Create a CEL v4 file of the same chip type
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
createCel(outFile, hdr, overwrite=TRUE)
str(readCelHeader(outFile))

# Verify correctness by update and re-read a few cells
intensities <- as.double(1:100)
indices <- seq(along=intensities)
updateCel(outFile, indices=indices, intensities=intensities)
value <- readCel(outFile, indices=indices)$intensities
stopifnot(identical(intensities, value))

# Clean up
#rm(files, file, hdr, hdr2)
} # for (zzz in 0)

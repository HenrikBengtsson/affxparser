for (zzz in 0) {  # Only so that 'break' can be used

# Scan current directory for CEL files
files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
if (length(files) == 0)
  break;

file <- files[1]

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Read a subset of cells
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
indices <- c(1:5, 1250:1500, 450:440)
cel <- readCel(file, indices=indices, readOutliers=TRUE)
str(cel)



# Clean up
rm(files, file, cel)

} # for (zzz in 0)

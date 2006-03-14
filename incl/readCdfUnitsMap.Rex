
for (zzz in 0) {

# Find any CDF file
cdfFile <- findCdf()
if (is.null(cdfFile))
  break;

# Create a cell-index map (for reading)
map <- readCdfUnitsMap(cdfFile)

# Inverse map to be used to write cell elements such that, if they 
# later are read unit by unit, they are read in contiguous blocks.
writeMap <- match(1:length(map), map)  # faster than order()

# Validate the two maps
stopifnot(identical(map[writeMap], 1:length(map)))


cat("Summary of the \"randomness\" of the cell indices:\n")
moves <- diff(map) - 1
cat(sprintf("Number of unnecessary file re-positioning: %d (%.1f%%)\n", 
                   sum(moves != 0), 100*sum(moves != 0)/length(moves)))
cat(sprintf("Extra positioning in Gb: %.1f\n", sum(abs(moves))/1024^3))

smallMoves <- moves[abs(moves) <= 25];
largeMoves <- moves[abs(moves)  > 25];
layout(matrix(1:2))
main <- "Non-signed file moves required in unorded file"
hist(smallMoves, nclass=51, main=main, xlab="moves <=25 bytes")
hist(largeMoves, nclass=101, main="", xlab="moves >25 bytes")

# Clean up
rm(cdfFile, map, writeMap, moves, smallMoves, largeMoves, main)
} # for (zzz in 0)

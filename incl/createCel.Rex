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

hdr2 <- readCelHeader(outFile)

keep <- setdiff(names(hdr), "filename")
hdr <- hdr[keep]
hdr2 <- hdr2[keep]
trim <- function(s) gsub("^[ \t\n]*", "", gsub("[ \t\n]*$", "", s));
for (kk in seq(along=hdr)) {
  value1 <- trim(hdr[[kk]]);
  value2 <- trim(hdr2[[kk]]);
  if (names(hdr)[kk] == "header") {

  }
  equals <- identical(agrep(value1, value2), 1:1)
  if (!equals) {
    print(value1)
    print(value2)
    stop(sprintf("Header field '%s' differs.", names(hdr)[kk]))
  }
}

# Clean up
#rm(files, file, hdr, hdr2)
} # for (zzz in 0)

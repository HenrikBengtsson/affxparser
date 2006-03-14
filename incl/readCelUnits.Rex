
# Scan current directory for CEL files
files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")

if (length(files) > 0) {
  cel <- readCelUnits(files[1], units=100:105)
  print(cel)
  rm(cel)
}

# Clean up
rm(files)

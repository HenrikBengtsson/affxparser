library(affxparser);

logMemory <- exists("memory.size", mode="function");
memSizeRange <- NA;

celFiles <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
nbrOfFiles <- length(celFiles);
if (nbrOfFiles == 0)
  stop("No CEL files found");

cat(sprintf("Found %d CEL file(s)\n", nbrOfFiles));

for (kk in 1:10000) {
  cat(sprintf("Round %d:\n", kk));

  # Sample a CEL file
  celFile <- celFiles[sample(nbrOfFiles, size=1)];

  # Reading CEL header
  t <- system.time({
  header <- readCelHeader(celFile);
  }, gcFirst=FALSE)[3]

  cat(sprintf("Number header elements read: %d\n", length(header)));
  cat(sprintf("Read time: %.2fms\n", 1000*t));
  size <- object.size(header);
  cat(sprintf("Size: %.3gkB\n", size/1024));

  if (logMemory) {
    memSize <- memory.size();
    memSizeRange <- range(memSizeRange, memSize, na.rm=TRUE);
    cat(sprintf("Memory size: %.2fMB in [%.2fMB, %.2fMB]\n", 
            memSize/1024^2, memSizeRange[1]/1024^2, memSizeRange[2]/1024^2));
  }
  cat("\n");
}

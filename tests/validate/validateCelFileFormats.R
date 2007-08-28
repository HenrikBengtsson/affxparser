###########################################################################
# Test for different CEL file formats
#
# This script loads the Test3 CEL files in ASCII, binary, and in Calvin
# formats and asserts that the read data is identical across file formats.
###########################################################################
library("affxparser");
library("AffymetrixDataTestFiles");


# Search for some CEL files
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE, firstOnly=FALSE)
files <- grep("FusionSDK_Test3", files, value=TRUE)
nbrOfFiles <- length(files);
if (nbrOfFiles == 0)
  stop("No CEL files found");
cat(sprintf("Found %d CEL file(s)\n", nbrOfFiles));

# Split them up in XDA, Calvin, and ASCII.
celFiles <- list();
for (format in c("XDA", "Calvin", "ASCII")) {
  tmp <- grep(format, files, value=TRUE);
  if (length(tmp) > 0)
    celFiles[[format]] <- tmp;
}
files <- celFiles;
print(files);


# Read all CEL files
cel <- list();
for (format in names(files)) {
  cel[[format]] <- readCelUnits(files[[format]]);
}

# Assert that CEL files for different file formats give identical results
for (format1 in names(cel)) {
  for (format2 in setdiff(names(cel), format1)) {
    stopifnot(identical(cel[[format1]], cel[[format2]]));
  }
}


# Plot spatial distribution intensities 
rotate270 <- function(x, ...) { 
  x <- t(x)
  nc <- ncol(x)
  if (nc < 2) return(x)
  x[,nc:1,drop=FALSE]
}

layout(matrix(1:6, ncol=2, byrow=TRUE));
opar <- par(mar=c(1,6,1.5,5)+0.1);
for (format in names(files)) {
  for (file in files[[format]]) {
    cel <- readCelRectangle(file);
    z <- rotate270(cel$intensities);
    image(z, col=gray.colors(256), axes=FALSE)
    title(main=sprintf("%s [%s]", basename(file), format), line=+0.75);
    chiptype <- paste("Chip type:", cel$header$chiptype);
    text(x=0, y=0.5, labels=chiptype, srt=90, adj=c(0.5,-1), xpd=TRUE)
    text(x=0, y=1, labels="(0,0)", adj=c(0,-0.7), cex=0.8, xpd=TRUE);
    lrLabel <- sprintf("(%d,%d)", cel$header$cols, cel$header$rows);
    text(x=1, y=0, labels=lrLabel, adj=c(1,1.2), cex=0.8, xpd=TRUE);
  }
}
par(opar);

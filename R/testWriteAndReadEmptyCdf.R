## out <- system('R -e "affxparser:::.testWriteAndReadEmptyCdf()"', intern=TRUE)
## res <- any(regexpr("COMPLETE", out) != -1);
.testWriteAndReadEmptyCdf <- function() {
  pathname <- system.file("testscripts/writeAndReadEmptyCdf.R", package="affxparser");
  cat("Pathname: ", pathname, "\n", sep="");

  source(pathname, echo=TRUE);
  
  cat("COMPLETE\n");
} # .testWriteAndReadEmptyCdf()


############################################################################
# HISTORY:
# 2012-05-18
# o Added because of the OSX build bug, cf.
#   https://groups.google.com/d/topic/aroma-affymetrix/lEfDanThLEA/discussion
# o Created.
############################################################################

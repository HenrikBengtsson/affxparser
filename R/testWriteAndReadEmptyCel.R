## out <- system('R -e "affxparser:::.testWriteAndReadEmptyCel()"', intern=TRUE)
## res <- any(regexpr("COMPLETE", out) != -1);
.testWriteAndReadEmptyCel <- function() {
  pathname <- system.file("testscripts/writeAndReadEmptyCel.R", package="affxparser");
  cat("Pathname: ", pathname, "\n", sep="");

  source(pathname, echo=TRUE);
  
  cat("COMPLETE\n");
} # .testWriteAndReadEmptyCel()


############################################################################
# HISTORY:
# 2012-09-26
# o Created from testWriteAndReadEmptyCdf.R.
############################################################################

out <- system('R -e "affxparser:::.testWriteAndReadEmptyCdf()"', intern=TRUE);
cat(out, sep="\n");
res <- any(regexpr("COMPLETE", out) != -1);
cat("Test result: ", res, "\n", sep="");


############################################################################
# HISTORY:
# 2012-05-18
# o Added because of the OSX build bug, cf.
#   https://groups.google.com/d/topic/aroma-affymetrix/lEfDanThLEA/discussion
# o Created.
############################################################################

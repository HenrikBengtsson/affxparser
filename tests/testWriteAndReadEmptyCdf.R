systemR <- function(command="", ..., verbose=FALSE) {
  # Locate the R executable
  Rbin <- file.path(R.home("bin"), "R")
  cmd <- sprintf('%s %s', shQuote(Rbin), command)
  if (verbose) cat("Command: ", cmd, "\n", sep="")
  system(cmd, ...)
} # systemR()


## Explicitly append 'affxparser' to library path
## Needed for covr::coverage()
pd <- packageDescription("affxparser")
libpath <- dirname(dirname(dirname(attr(pd, "file"))))
cmd <- sprintf(' -e ".libPaths(\'%s\'); affxparser:::.testWriteAndReadEmptyCdf()"', libpath)
out <- systemR(cmd, intern=TRUE, wait=TRUE, verbose=TRUE)
cat(out, sep="\n")
res <- any(regexpr("COMPLETE", out) != -1)
cat("Test result: ", res, "\n", sep="")
if (!res) {
  stop("affxparser:::.testWriteAndReadEmptyCdf() failed.")
}

############################################################################
# HISTORY:
# 2012-05-22
# o ROBUSTNESS: Now launching R without assuming it is on the search path,
#   cf. R-devel thread 'Best way to locate R executable from within R?'
#   on May 22, 2012.
# 2012-05-18
# o Added because of the OSX build bug, cf.
#   https://groups.google.com/d/topic/aroma-affymetrix/lEfDanThLEA/discussion
# o Created.
############################################################################

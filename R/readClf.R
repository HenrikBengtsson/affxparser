readClfHeader <- function(file) {
  as.list(readClfEnv(file, readBody=FALSE));
}

readClf <- function(file) {
  as.list(readClfEnv(file, readBody=TRUE));
}

readClfEnv <- function(file, readBody=TRUE) {
  ## FIXME: this is an exception in more recent TsvFile.cpp
  # Argument 'file':
  if (!file.exists(file)) {
    stop("Cannot read CLF file. File not found: ", file); 
  }

  env <- new.env(parent=emptyenv());
  res <- .Call("R_affx_get_clf_file", file, readBody, env, PACKAGE="affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read CLF file: ", file);
  }

  res;
} # readClfEnv()



############################################################################
# HISTORY:
# 2012-06-14 [HB]
# o Extracted all CLF functions to readClf.R.
# o Harmonized the error messages.
# 2011-11-18 [HB]
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2008-03-24 [JB]
# o ROBUSTNESS: Asserts that argument 'indices' to readClfEnv() and
#   readPgfEnv() is ordered.
# 2008-03-22 [JB]
# o Added read(Clf|Pgf)Env() functions.
# 2008-03-04 [MM]
# o Added CLF and PGF parsers by Martin Morgan (MM).
############################################################################

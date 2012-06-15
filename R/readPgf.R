readPgfHeader <- function(file) {
  as.list(readPgfEnv(file, readBody=FALSE));
}

readPgf <- function(file, indices=NULL) {
  as.list(readPgfEnv(file, readBody=TRUE, indices=indices));
}

readPgfEnv <- function(file, readBody=TRUE, indices=NULL) {
  ## FIXME: this is an exception in more recent TsvFile.cpp
  # Argument 'file':
  if (!file.exists(file)) {
    stop("Cannot read PGF file. File not found: ", file); 
  }

  # Argument 'indices':
  if (!is.null(indices)) {
    if (!all(sort(indices) == indices)) {
      stop("Argument 'indices' must be sorted.");
    }
  }

  env <- new.env(parent=emptyenv());
  res <- .Call("R_affx_get_pgf_file", file, readBody, env, indices,
               PACKAGE="affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read PGF file: ", file);
  }

  res;
} # readPgfEnv()


.pgfProbeIndexFromProbesetIndex <- function(pgf, probesetIdx) {
  ## probeset -> atom
  astarts <- pgf[["probesetStartAtom"]][probesetIdx];
  aends <- pgf[["probesetStartAtom"]][probesetIdx+1];
  aends[is.na(aends)] <- length(pgf[["atomId"]]) + 1;
  ## atom -> probe
  pstarts <- pgf[["atomStartProbe"]][astarts];
  pends <- pgf[["atomStartProbe"]][aends] - 1;
  pends[is.na(pends)] <- length(pgf[["probeId"]]);
  mapply(":", pstarts, pends, SIMPLIFY=FALSE);
} # .pgfProbeIndexFromProbesetIndex()


############################################################################
# HISTORY:
# 2012-06-14 [HB]
# o readPgfEnv(..., indices=NULL) no longer gives a warning.
# o Moved all CLF functions to readClf.R.
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

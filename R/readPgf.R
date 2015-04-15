readPgfHeader <- function(file) {
  as.list(readPgfEnv(file, readBody=FALSE));
}

readPgf <- function(file, indices=NULL) {
  # UNSUPPORTED CASE
  ## FIXME: When we have confirmed that no dependencies assumes
  ##        !is.null(indices) && length(indices) == ) to read
  ##        all probesets, then we can remove this. /HB 2014-11-22
  if (!is.null(indices) && length(indices) == 0L) {
    stop("readPgf(..., indices=integer(0)) is not supported.")
  }

  as.list(readPgfEnv(file, readBody=TRUE, indices=indices));
}

readPgfEnv <- function(file, readBody=TRUE, indices=NULL) {
  ## Local functions
  coercePgfHeader <- function(res, ...) {
    header <- res$header
    if (!is.list(header)) return(res)
    ## Optional fields to coerce to integers
    fields <- c("num-cols", "num-rows", "probesets", "datalines")
    fields <- intersect(fields, names(header)) # optional!
    for (field in fields) {
      value <- header[[field]]
      value <- as.integer(value)
      header[[field]] <- value
    }
    res$header <- header
    res
  } # coercePgfHeader()


  ## FIXME: this is an exception in more recent TsvFile.cpp
  # Argument 'file':
  if (!file.exists(file)) {
    stop("Cannot read PGF file. File not found: ", file);
  }

  # Argument 'indices':
  if (!is.null(indices)) {
    indices <- as.integer(indices)
    if (any(indices <= 0L)) {
      stop("Argument 'indices' contains a non-positive element")
    }
    if (!all(sort(indices) == indices)) {
      stop("Argument 'indices' must be sorted.");
    }
  }

  # UNSUPPORTED CASE
  ## FIXME: When we have confirmed that no dependencies assumes
  ##        !is.null(indices) && length(indices) == ) to read
  ##        all probesets, then we can remove this. /HB 2014-11-22
  if (!is.null(indices) && length(indices) == 0L) {
    stop("readPgf(..., indices=integer(0)) is not supported.")
  }

  ## Read all of PGF file?
  if (is.null(indices)) {
    env <- new.env(parent=emptyenv());
    res <- .Call("R_affx_get_pgf_file", file, readBody, env, NULL,
                 PACKAGE="affxparser");
    if (is.null(res)) stop("Failed to read PGF file: ", file)
    res <- coercePgfHeader(res)
  } else {
    ## Read file header
    env <- new.env(parent=emptyenv())
    res <- .Call("R_affx_get_pgf_file", file, FALSE, env, NULL,
                 PACKAGE="affxparser")
    if (is.null(res)) stop("Failed to read PGF file: ", file)
    res <- coercePgfHeader(res)
    # Validate indices?
    nbrOfUnits <- res$header$probesets
    if (is.numeric(nbrOfUnits)) {
      if (any(indices > nbrOfUnits)) {
        stop(sprintf("Argument 'indices' is out of range [1,%d]", nbrOfUnits))
      }
    }
    res <- .Call("R_affx_get_pgf_file", file, readBody, env, indices,
                 PACKAGE="affxparser")
    res <- coercePgfHeader(res)
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
# 2015-04-15 [HB]
# o ROBUSTNESS: Now readPgfEnv()/readPgf() validated 'indices', iff possible.
# o Now readPgfEnv()/readPgf() coerces some header fields to integers.
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

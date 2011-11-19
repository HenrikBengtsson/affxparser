readCdfHeader <- function(filename) {
  # Expand '~' pathnames to full pathnames.
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename))
    stop("Cannot read CDF header. File not found: ", filename);

  res <- .Call("R_affx_get_cdf_file_header", filename, PACKAGE="affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read CDF file header: ", filename);
  }

  # Adding backward compatibility for now. /HB 2006-09-07
  res$rows <- res$nrows;
  res$cols <- res$ncols;
  res$probesets <- res$nunits;
  res$qcprobesets <- res$nqcunits;
  res$reference <- res$refseq;

  # Workaround for a bug in Fusion SDK. /HB 2008-12-04
  # If the CDF file is on a Windows share, then the from
  # pathname inferred chip type contains a path as well.
  # Reference: See aroma.affymetrix thread on Dec 4, 2008.
  res$chiptype <- basename(res$chiptype);

  res;
} # readCdfHeader()


############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
############################################################################

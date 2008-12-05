readCdfHeader <- function(filename) {
  # Expand '~' pathnames to full pathnames.
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename))
    stop("Cannot read CDF header. File not found: ", filename);

  res <- .Call("R_affx_get_cdf_file_header", filename, PACKAGE="affxparser");

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
}

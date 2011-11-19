readBpmapHeader <- function(filename) {
  res <- .Call("R_affx_get_bpmap_header", filename);

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read BPMAP file header: ", filename);
  }

  res;
} # readBpmapHeader()

############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
############################################################################

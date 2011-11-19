readBpmapSeqinfo <- function(filename, seqIndices=NULL, verbose=0) {
  res <- .Call("R_affx_get_bpmap_seqinfo", filename, as.integer(seqIndices), as.integer(verbose), PACKAGE = "affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read sequence information from BPMAP file: ", filename);
  }

  res;
} # readBpmapSeqinfo()

############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
############################################################################

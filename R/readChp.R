readChp <- function(filename, withQuant=TRUE) {
  res <- .Call("R_affx_get_chp_file", filename, withQuant);

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read CHP file: ", filename);
  }

  res;
} # readChp()

############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
############################################################################

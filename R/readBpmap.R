readBpmap <- function(filename, seqIndices = NULL, readProbeSeq = TRUE,
                      readSeqInfo = TRUE, readPMXY = TRUE,
                      readMMXY = TRUE, readStartPos = TRUE,
                      readCenterPos = FALSE, readStrand = TRUE,
                      readMatchScore = FALSE, readProbeLength = FALSE,
                      verbose = 0) {
  res <- .Call("R_affx_get_bpmap_file", filename, as.integer(seqIndices),
          readSeqInfo, readStartPos, readCenterPos, readProbeSeq,
          readStrand, readPMXY, readMMXY, readMatchScore, readProbeLength,
          as.integer(verbose), PACKAGE = "affxparser");

  # Sanity check
  if (is.null(res)) {
    stop("Failed to read BPMAP file: ", filename);
  }
   
  res;
} # readBpmap()


############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
############################################################################


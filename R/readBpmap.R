readBpmap <- function(filename, seqIndices = NULL, readProbeSeq = TRUE,
                      readSeqInfo = TRUE, readPMXY = TRUE,
                      readMMXY = TRUE, readStartPos = TRUE,
                      readCenterPos = FALSE, readStrand = TRUE,
                      readMatchScore = FALSE, readProbeLength = FALSE,
                      verbose = 0){
    .Call("R_affx_get_bpmap_file", filename, as.integer(seqIndices),
          readSeqInfo, readStartPos, readCenterPos, readProbeSeq,
          readStrand, readPMXY, readMMXY, readMatchScore, readProbeLength,
          as.integer(verbose), PACKAGE = "affxparser");
}

readBpmapSeqinfo <- function(filename, seqIndices = NULL, verbose = 0){
    seqInfo <- .Call("R_affx_get_bpmap_seqinfo", filename, as.integer(seqIndices),
                     as.integer(verbose), PACKAGE = "affxparser");
    seqInfo
}


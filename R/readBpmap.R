readBpmap <- function(filename, seqIndices = NULL, verbose = 0){
    .Call("R_affx_get_bpmap_file", filename, as.integer(seqIndices), as.integer(verbose),
          PACKAGE = "affxparser");
}

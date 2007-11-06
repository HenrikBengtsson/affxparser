tpmap2bpmap <- function(tpmapname, bpmapname, verbose = 0){
    invisible(.Call("R_affx_write_bpmap_file", bpmapname, tpmapname, as.integer(verbose)))
}

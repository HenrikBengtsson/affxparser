readBpmap <- function(filename, verbose = 0){
    bpmap <- .Call("R_affx_get_bpmap_file", filename, as.integer(verbose),
                   PACKAGE = "affxparser");
    bpmap
}

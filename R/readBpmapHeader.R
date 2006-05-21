readBpmapHeader <- function(filename){
    .Call("R_affx_get_bpmap_header", filename)
}

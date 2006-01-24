read.cdf.header <- function(fname){
    fname <- file.path(dirname(fname), basename(fname))
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))
    return(.Call("R_affx_get_cdf_file_header", 
                 as.character(fname), 
                 PACKAGE="affxparser"))
}

read.cdf.complete <- function(fname, verbose = 0){
    fname <- file.path(dirname(fname), basename(fname))
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))
    return(.Call("R_affx_get_cdf_file",
                 as.character(fname),
                 as.integer(verbose), 
                 PACKAGE="affxparser"))
}


############################################################################
# HISTORY:
# 2006-01-10
# o Added PACKAGE="affxparser" to all .Call() calls. /HB
############################################################################  

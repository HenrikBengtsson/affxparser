read.cdf.header <- function(fname){
    fname <- file.path(dirname(fname), basename(fname))
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))
    return(.Call("R_affx_get_cdf_file_header", as.character(fname)))
}

read.cdf.complete <- function(fname, verbose = 0){
    fname <- file.path(dirname(fname), basename(fname))
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))
    return(.Call("R_affx_get_cdf_file",
                 as.character(fname),
                 as.integer(verbose)))
}

read.cdf.env <- function(fname, complementary.logic = TRUE, verbose = 0) {
    fname <- file.path(dirname(fname), basename(fname))
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))
    pmmm <- .Call("R_affx_get_pmmm_list",
                  as.character(fname),
                  complementary.logic,
                  verbose = verbose)
    if (is.null(pmmm) || length(pmmm) == 0)
        stop(paste("Error parsing:", fname))
    pmmm <- lapply(pmmm, function(x) {
        tmp <- t(x)
        colnames(tmp) <- c("pm", "mm")
        tmp})
    e <- new.env(hash = TRUE)
    multiassign(names(pmmm), value = pmmm, e)
    return(e)
}

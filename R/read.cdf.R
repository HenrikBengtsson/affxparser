read.cdf.complete <- function(fname, verbose = 0){
  fname <- path.expand(fname)
  if(!file.exists(fname))
    stop(paste(fname, "does not exist."))

  .Call("R_affx_get_cdf_file", as.character(fname), as.integer(verbose))
}

read.cdf.header <- function(fname, verbose = 0){
  fname <- path.expand(fname)
  if(!file.exists(fname))
    stop(paste(fname, "does not exist."))

  .Call("R_affx_get_cdf_file_header", as.character(fname), as.integer(verbose))
}


read.cdf.env <- function(fname) {
  pmmm <- .parse.cdffile(fname, complementary.logic)
  e <- new.env()
  multiassign(names(pmmm), value = pmmm, e)
  e
}

.get.cdf.env <- function(fname) {
  fname <- file.path(dirname(fname), basename(fname))

  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))

  pmmm <- .Call("R_affx_get_pmmm_list", as.character(fname), as.integer(complementary.logic))

  if (is.null(pmmm) || length(pmmm) == 0)
    stop(paste("Error parsing:", fname))
  
  lapply(pmmm, t)
}

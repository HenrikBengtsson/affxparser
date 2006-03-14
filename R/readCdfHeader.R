readCdfHeader <- function(fname){
  fname <- file.path(dirname(fname), basename(fname))
  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))
  return(.Call("R_affx_get_cdf_file_header", 
               as.character(fname), 
               PACKAGE="affxparser"))
}

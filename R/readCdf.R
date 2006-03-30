readCdf <- function(filename, verbose = 0){
  filename <- file.path(dirname(filename), basename(filename))
  if (!file.exists(filename))
    stop(paste("file:", filename, "does not exist."))
  return(.Call("R_affx_get_cdf_file",
               as.character(filename),
               as.integer(verbose), 
               PACKAGE="affxparser"))
}

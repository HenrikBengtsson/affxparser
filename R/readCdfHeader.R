readCdfHeader <- function(filename){
  # Expand '~' pathnames to full pathnames.
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename))
    stop("Cannot read CDF header. File not found: ", filename);

  .Call("R_affx_get_cdf_file_header", filename, PACKAGE="affxparser");
}

readCelHeader <- function(filename) {
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Validate arguments
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Argument 'filename':
    if(length(filename) != 1) {
      stop("Argument 'filename' should be a single file: ", 
                                            paste(filename, collapse=", "));
    }
    # Expand '~' pathnames to full pathnames.
    filename <- file.path(dirname(filename), basename(filename));
    if (!file.exists(filename)) {
      stop("Cannot read CEL file header. File not found: ", filename);
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Reading CEL header
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    .Call("R_affx_get_cel_file_header", filename, PACKAGE="affxparser");
} # readCelHeader()

########################################################################/**
# @RdocFunction isCelFile
#
# @title "Checks if a file is a CEL file or not"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{filename}{A filename.}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns @TRUE if a CEL file, otherwise @FALSE.  
#  If file does not exist, an exception is thrown.
# }
#
# \seealso{
#  @see "readCel", @see "readCelHeader", @see "readCelUnits".
# }
#
# @author
#
# @keyword programming
# @keyword file
# @keyword IO
# @keyword internal
#**/#######################################################################
isCelFile <- function(filename, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Argument 'filename':
  if (!file.exists(filename)) {
    stop("Cannot check file format. File not found: ", filename);
  }

  # Open file

  fh <- NULL;
  header <- NULL;

  # Try to read CEL v4 header
  tryCatch({
    fh <- file(filename, open="rb");
    header <- .readCelHeaderV4(fh);
    close(fh);
    fh <- NULL;
  }, error = function(ex) {
    if (!is.null(fh) && isOpen(fh))
      on.exit(close(fh));
  })

  if (!is.null(header))
    return(TRUE);

  # Try to read CEL v3 header
  tryCatch({
    fh <- file(filename, open="r");
    header <- .readCelHeaderV3(fh);
    close(fh);
    fh <- NULL;
  }, error = function(ex) {
    if (!is.null(fh) && isOpen(fh))
      on.exit(close(fh));
  })

  if (!is.null(header))
    return(TRUE);
    
  FALSE;
}


############################################################################
# HISTORY:
# 2006-07-27
# o BUG FIX: The error message when the file was not found was broken.
# 2006-07-10
# o Created.
############################################################################  

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
#  ASCII (v3), binary (v4;XDA), and binary (CCG v1;Calvin) CEL files 
#  are recognized.
#  If file does not exist, an exception is thrown.
# }
#
# \seealso{
#  @see "readCel", @see "readCelHeader", @see "readCelUnits".
# }
#
# @author "HB"
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

  # Close an open connections on exit
  con <- NULL;
  on.exit({
   if (inherits(con, "connection") && isOpen(con))
     on.exit(close(con));
  })

  # Note, we cannot use readCelHeader(), because that will crash R if
  # it is not a CEL file.

  header <- NULL;
  for (ver in c("4", "3", "1")) {
    tryCatch({
      if (inherits(con, "connection") && isOpen(con)) {
        close(con);
        con <- NULL;
      }
      con <- file(filename, open="rb");
      if (ver == "4") {
        header <- .readCelHeaderV4(con);
      } else if (ver == "3") {
        header <- .readCelHeaderV3(con);
      } else {
        header <- readCcgHeader(con);
        dataTypeId <- header$dataHeader$dataTypeId;
        if (!identical(dataTypeId, "affymetrix-calvin-intensity"))
          header <- NULL;
      }
    }, error = function(ex) { 
#      print(ex);
    })
  }

  isCelFile <- (!is.null(header));
    
  isCelFile;
}


############################################################################
# HISTORY:
# 2007-08-16
# o Updated isCelFile() so it returns TRUE for CCG CEL v1 files.
# 2006-07-27
# o BUG FIX: The error message when the file was not found was broken.
# 2006-07-10
# o Created.
############################################################################  

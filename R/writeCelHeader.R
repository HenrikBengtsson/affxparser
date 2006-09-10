#########################################################################/**
# @RdocFunction writeCelHeader
#
# @title "Writes a CEL header to a connection"
#
# @synopsis 
# 
# \description{
#   @get "title".
# }
# 
# \arguments{
#   \item{con}{A @connection.}
#   \item{header}{A @list structure describing the CEL header, similar
#     to the structure returned by @see "readCelHeader".}
#   \item{...}{Not used.}
# }
# 
# \value{
#   Returns (invisibly) the pathname of the file created.
# }
#
# \details{
#   Currently only CEL v4 headers are supported.
# }
#
# \section{Redundant fields}{
#   The CEL v4 header contains redundant information. To avoid inconsistency
#   this method generates such redundant values from the original values.
#   This is consistent to how the CEL reader in Fusion SDK does it, cf.
#   @see "readCelHeader".  The redundant information is in the (CEL v3)
#   \code{header} field, which contains the CEL header information as it
#   would appear in the CEL v3 format.  This in turn contains a DAT header
#   field reproducing the DAT header from the image analysis.  It is from
#   this DAT header that the chip type is extracted.
# }
#
# @author
#
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*/#########################################################################
writeCelHeader <- function(con, header, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  writeInteger <- function(con, data, ...) {
    writeBin(con=con, as.integer(data), size=4, endian="little");
  }
  
  writeString <- function(con, str, ...) {
    writeInteger(con=con, nchar(str)+1);
    writeChar(con=con, str);
  }
  
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Open CEL file
  if (!inherits(con, "connection"))
    stop("Argument 'con' must be a connection: ", class(con));


  # Check for supported versions
  if (header$version !=  "4") {
    stop("Failed write CEL v4 header. Currently only v4 is supported: ", 
                                                           header$version);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate structure
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Number of columns and rows, and total number of cells
  rows <- header$rows;
  cols <- header$cols;
  if (rows < 1)
    stop("Number of rows must be at least one: ", rows);
  if (cols < 1)
    stop("Number of columns must be at least one: ", cols);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Unwrap/wrap header
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
str(header);
  # First, try to unwrap CEL header in case it isn't.
  tryCatch({
    header <- .unwrapCelHeaderV4(header);
  }, error = function(ex) {
    # If this happens, we assumes that the header was already unwrapped.
  });

str(header);
  # Then wrap it up again to make sure it has the right format.  This will
  # also override redundant fields.
  header <- .wrapCelHeaderV4(header);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Write to file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Magic number (always set to 64)
  writeInteger(con=con, 64);

  # Version number (always set to 4)
  writeInteger(con=con, 4);

  writeInteger(con=con, c(rows, cols, rows*cols));

  # "Header as defined in the HEADER section of the version 3 CEL 
  #  files. The string contains TAG=VALUE separated by a space where
  #  the TAG names are defined in the version 3 HEADER section."
  # Note that Fusion SDK and hence readCelHeader() ignores the Algorithm
  # and Algorithm Parameters part of this header; instead it reads it
  # from the CEL header (below) and adds it to this when returned.
  # Thus, if you wish to update any of these two fields you need to udate
  # the ones below (in addition to here?!?).
  writeString(con=con, header$header);

  # "The algorithm name used to create the CEL file".
  writeString(con=con, header$algorithm);

  # "The parameters used by the algorithm. The format is TAG:VALUE
  #  pairs separated by semi-colons or TAG=VALUE pairs separated 
  #  by spaces."
  writeString(con=con, header$parameters);

  # "Cell margin used for computing the cells intensity value."
  writeInteger(con=con, header$cellmargin);

  # "Number of outlier cells."
  writeInteger(con=con, header$noutliers);

  # "Number of masked cells."
  writeInteger(con=con, header$nmasked);
} # writeCelHeader()


############################################################################
# HISTORY:
# 2006-09-07
# o With help of all the private unwrap and wrap functions, it seems to
#   work now.
# 2006-09-03
# o Created.
############################################################################

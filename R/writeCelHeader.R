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
#   \item{outputFormat}{A @character string specifying the output format.
#     Currently only CEL version 4 (binary;XDA) are supported.}
#   \item{...}{Not used.}
# }
# 
# \value{
#   Returns (invisibly) the pathname of the file created.
# }
#
# \details{
#   Currently only CEL version 4 (binary;XDA) headers can be written.
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
# @author "HB"
#
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*/#########################################################################
writeCelHeader <- function(con, header, outputVersion=c("4"), ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  writeDWord <- function(con, data, ...) {
    writeBin(con=con, as.integer(data), size=4, endian="little");
  }
  
  writeInteger <- function(con, data, ...) {
    writeBin(con=con, as.integer(data), size=4, endian="little");
  }
  
  writeString <- function(con, str, ...) {
    # Strings must not be null terminated! /HB 2006-09-10
    writeInteger(con=con, nchar(str));
    writeChar(con=con, str, eos=NULL);
  }

  writeCelHeaderV4 <- function(con, header, ...) {
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Validation
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Get the version of the CEL header
    version <- .getCelHeaderVersion(header);
    if (version ==  "1") {
      # A CEL header in the Calvin CEL file format
      header <- .getCelHeaderV4(header);
    } else if (version == "3") {
      # A CEL header in the ASCII (v3) CEL file format
      stop("Failed to write CEL v4 header. Argument 'header' is a CEL v3 header, which is not supported: ", version);
    } else if (version == "4") {
      # A CEL header in the XBR (v4) CEL file format
      header <- .getCelHeaderV4(header);
    } else {
      stop("Failed to write CEL v4 header. Unsupported CEL header version: ", version);
    }

    # Number of columns and rows, and total number of cells
    if (header$rows < 1)
      stop("Number of rows must be at least one: ", rows);
    if (header$cols < 1)
      stop("Number of columns must be at least one: ", cols);
  

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Unwrap/wrap CEL header
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # First, try to unwrap CEL header in case it isn't.
    if (header$version == "4") {
      tryCatch({
        header <- .unwrapCelHeaderV4(header);
      }, error = function(ex) {
        # If this happens, we assumes that the header was already unwrapped.
      });
    }
  
    # Then wrap it up again to make sure it has the right format.  This will
    # also override redundant fields.
    header <- .wrapCelHeaderV4(header);


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Write to connection
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Magic number (always set to 64)
    writeInteger(con=con, 64);

    # Version number (always set to 4)
    writeInteger(con=con, 4);

    rows <- header$rows;
    cols <- header$cols;
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
    writeDWord(con=con, header$noutliers);

    # "Number of masked cells."
    writeDWord(con=con, header$nmasked);
  } # writeCelHeaderV4()

  
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Open CEL file
  if (!inherits(con, "connection"))
    stop("Argument 'con' must be a connection: ", class(con));

  # Argument 'outputVersion':
  outputVersion <- match.arg(outputVersion);
  if (outputVersion == "4") {
    writeCelHeaderV4(con=con, header=header, ...);
  } else {
    stop("Unsupported output version: ", outputVersion);
  }
} # writeCelHeader()


############################################################################
# HISTORY:
# 2007-08-16
# o Now writeCelHeader() can write CEL header given CCG v1 CEL headers,
#   in addition to v4 headers.
# o Added argument 'outputVersion' to writeCelHeader().  It still only
#   supports version 4, though.
# 2007-08-15
# o Created internal function writeCelHeaderV4().
# 2006-09-10
# o BUG FIX: A hard to find bug was that strings in the CEL header must 
#   *not* be written with a null terminator.
# 2006-09-07
# o With help of all the private unwrap and wrap functions, it seems to
#   work now.
# 2006-09-03
# o Created.
############################################################################

.writeCcgFileHeader <- function(con, header, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  writeUByte <- function(con, value, ...) {
    writeBin(as.integer(value), con=con, size=1, endian="big");
  }

  writeInt <- function(con, value, ...) {
    writeBin(as.integer(value), con=con, size=4, endian="big");
  }

  writeUInt <- function(con, value, ...) {
    writeBin(as.integer(value), con=con, size=4, endian="big");
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate 'header':
  if (!is.list(header))
    stop("Argument 'header' must be a list: ", mode(header));
  if (is.null(header$nbrOfDataGroups))
    stop("Missing element 'nbrOfDataGroups' in argument 'header'.");
  if (is.null(header$dataGroupStart))
    stop("Missing element 'dataGroupStart' in argument 'header'.");


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Further validation
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Default version is 1 (one) [1]
  if (is.null(header$version))
    header$version <- as.integer(1);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Writing
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # The magic for Command Console generic data file format is 59 [1]
  magic <- writeUByte(con, value=59);

  # Write version
  version <- writeUByte(con, value=header$version);

  # Write number of data groups
  nbrOfDataGroups <- writeInt(con, value=header$nbrOfDataGroups);

  # Write file position of first data groups
  dataGroupStart <- writeUInt(con, value=header$nbrOfDataGroups);

  # Return the current file write position
  invisible(seek(con, origin="start", rw="write"));
} # .writeCcgFileHeader()





# Generic Data Header
# This section stores the file and file type identifiers, data to
# describe the contents of the file, parameters on how it was created
# and information about its parentage. This section contains a circular
# dependency so as to traverse across the entire parentage of a file.
# This information will provide the entire history of how a file came
# to be.
#
# The first data header section immediately follows the file header
# section.
#
# Item Type  Description
# 1    GUID  The data type identifier. This is used to identify the type
#            of data stored in the file. For example:
#            * acquisition data (affymetrix-calvin-scan-acquisition)
#            * intensity data (tbd)
#            * expression results (tbd)
#            * genotyping results (tbd)
# 2    GUID  Unique file identifier. This is the identifier to use to
#            link the file with parent files. This identifier will be
#            updated whenever the contents of the file change.
#            Example: When a  user manually aligns the grid in a DAT file
#            the grid coordinates are updated in the DAT file and the file
#            is given a new file identifier.
# 3 DATETIME Date and time of file creation.
# 4   LOCALE The locale of the operating system that the file was created on.
# 5 	   INT The number of name/type/value parameters.
# 6 	 WVT[] Array of parameters stored as name/value/type triplets.
#            WVT[]=(WSTRING/VALUE/TYPE)[]
# 7 	   INT Number of parent file headers.
# 8 	 GDH[] Array of parent file headers. GDH[]=GenericDataHeader[]
.writeCcgDataHeader <- function(con, header, ...) {
  # To please R CMD check
  charToInt <- NULL; rm(list="charToInt");

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  writeUByte <- function(con, value, ...) {
    writeBin(as.integer(value), con=con, size=1, endian="big");
  }

  writeInt <- function(con, value, ...) {
    writeBin(as.integer(value), con=con, size=4, endian="big");
  }

  writeUInt <- function(con, value, ...) {
    writeBin(as.integer(value), con=con, size=4, endian="big");
  }

  writeString <- function(con, str, ...) {
    # A 1 byte character string. A string object is stored as an INT
    # (to store the string length) followed by the CHAR array (to store
    # the string contents).
    str <- as.character(str);
    nchars <- as.integer(nchar(str));
    writeInt(con, value=nchars);
    writeChar(str, con=con, nchars=nchars);
  }

  writeWChar <- function(object, con, nchars=nchar(object,type="chars"), ...) {
    # Tho bytes per character
    str <- as.character(object);

    # Convert to unicode characters
    n <- nchar(str);
    raw <- matrix(raw(2*n), nrow=2, ncol=n);
    bfr <- charToInt(strsplit(str, split="")[[1]]);
    raw[2,] <- as.raw(bfr);
    raw <- as.vector(raw);

    # Write raw buffer
    writeBin(raw, con=con);
  }

  writeWString <- function(con, str, ...) {
    # A UNICODE string. A string object is stored as an INT (to store the
    # string length) followed by the WCHAR array (to store the string
    # contents).
    str <- as.character(str);
    nchars <- as.integer(nchar(str));
    writeInt(con, value=nchars);
    writeWChar(str, con=con, nchars=nchars);
  }

  writeGuid <- function(con, id, ...) {
    writeString(con, str=id, ...);
  }

  writeDateTime <- function(con, timestamp, ...) {
    writeWString(con, str=timestamp, ...);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate 'header':
  if (!is.list(header))
    stop("Argument 'header' must be a list: ", mode(header));
  if (is.null(header$nbrOfDataGroups))
    stop("Missing element 'nbrOfDataGroups' in argument 'header'.");
  if (is.null(header$dataGroupStart))
    stop("Missing element 'dataGroupStart' in argument 'header'.");


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Further validation
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Default version is 1 (one) [1]
  if (is.null(header$version))
    header$version <- as.integer(1);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Writing
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Data type identifier
  writeGuid(con, id=header$dataTypeId);

  # Unique file identifier
  writeGuid(con, id=header$fileId);

  # Unique file identifier
  writeDateTime(con, id=header$timestamp);

  # Write version
  version <- writeUByte(con, value=header$version);

  # Write number of data groups
  nbrOfDataGroups <- writeInt(con, value=header$nbrOfDataGroups);

  # Write file position of first data groups
  dataGroupStart <- writeUInt(con, value=header$nbrOfDataGroups);

  # Return the current file write position
  invisible(seek(con, origin="start", rw="write"));
} # .writeCcgDataHeader()


############################################################################
# HISTORY:
# 2012-05-18
# o Now using stop() instead of throw().
# 2007-08-16
# o This file only contains a stub, so there is currently no
#    writeCcgHeader() or writeCcg().
# 2006-11-06
# o Created.
############################################################################

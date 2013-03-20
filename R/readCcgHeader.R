#########################################################################/**
# @RdocFunction readCcgHeader
# 
# @title "Reads an the header of an Affymetrix Command Console Generic (CCG) file"
# 
# @synopsis
# 
# \description{
#   @get "title".  
# }
# 
# \arguments{
#   \item{pathname}{The pathname of the CCG file.}
#   \item{verbose}{An @integer specifying the verbose level. If 0, the
#     file is parsed quietly.  The higher numbers, the more details.}
#   \item{.filter}{A @list.}
#   \item{...}{Not used.}
# }
# 
# \value{
#   A named @list structure consisting of ...
# }
# 
# @author "HB"
# 
#  \details{
#    Note, the current implementation of this methods does not utilize the
#    Affymetrix Fusion SDK library.  Instead, it is implemented in R from the
#    file format definition [1].
#  }
#
#  \seealso{
#    @see "readCcg".
#  }
# 
# \references{
#  [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
#      April, 2006.
#      \url{http://www.affymetrix.com/support/developer/}\cr
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCcgHeader <- function(pathname, verbose=0, .filter=list(fileHeader=TRUE, dataHeader=TRUE), ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'pathname':
  if (inherits(pathname, "connection")) {
    con <- pathname;
    pathname <- NA;
  } else {
    if (!file.exists(pathname))
      stop("File not found: ", pathname);
    con <- file(pathname, open="rb");
    on.exit(close(con));
  }

  # Argument '.filter':
  hasFilter <- FALSE;
  if (!is.null(.filter)) {
    if (!is.list(.filter)) {
      stop("Argument '.filter' must be a list: ", mode(.filter));
    }
    hasFilter <- TRUE;
  }

  header <- list(filename=pathname);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read file header
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  hdr <- .readCcgFileHeader(con);
  if (identical(.filter$fileHeader, TRUE) || is.list(.filter$fileHeader)) {
    header$fileHeader <- hdr;
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read the data header
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  hdr <- .readCcgDataHeader(con, .filter=.filter$dataHeader);
  if (identical(.filter$dataHeader, TRUE) || is.list(.filter$dataHeader)) {
    header$dataHeader <- hdr;
  }

  header;
} # readCcgHeader()



# File Header
# The file header section is the first section of the file. This 
# section is used to identify the type of file (i.e. Command Console
# data file), its version number (for the file format) and the number
# of data groups stored within the file. Information about the contents
# of the file such as the data type identifier, the parameters used to
# create the file and its parentage is stored within the generic data 
# header section.
# 
# Item 	Description 	Type
# 1 Magic number. A value to identify that this is a Command Console 
#    data file. The value will be fixed to 59. 	[UBYTE]
# 2 The version number of the file. This is the version of the file
#    format. It is currently fixed to 1. [UBYTE]
# 3 The number of data groups. [INT]
# 4 File position of the first data group. [UINT]
.readCcgFileHeader <- function(pathname, .filter=NULL, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  readByte <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=1, signed=TRUE, endian="big", n=n);
  }

  readUByte <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=1, signed=FALSE, endian="big", n=n);
  }

  readInt <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=4, signed=TRUE, endian="big", n=n);
  }

  readUInt <- function(con, n=1, ...) {
    # For 4-byte integers 'signed' can not be FALSE for readBin()
    readBin(con, what=integer(), size=4, signed=FALSE, endian="big", n=n);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'pathname':
  if (inherits(pathname, "connection")) {
    con <- pathname;
  } else {
    if (!file.exists(pathname))
      stop("File not found: ", pathname);
    con <- file(pathname, open="rb");
    on.exit(close(con));
  }

  # Argument '.filter':
  hasFilter <- FALSE;
  if (!is.null(.filter)) {
    if (!is.list(.filter)) {
      stop("Argument '.filter' must be a list: ", mode(.filter));
    }
    hasFilter <- TRUE;
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  magic <- readUByte(con);
  if (magic != 59)
    stop("File format error: Not a CCG file. Magic is not 59: ", magic);

  version <- readUByte(con);
  nbrOfDataGroups <- readInt(con);
  dataGroupStart <- readUInt(con);

  list(
    version = version,
    nbrOfDataGroups = nbrOfDataGroups,
    dataGroupStart = dataGroupStart
  )
} # .readCcgFileHeader()



.readCcgDataHeader <- function(con, .filter=NULL, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  rawToString <- function(raw, ...) {
    # This approach drops all '\0', in order to avoid warnings
    # in rawToChar().  Note, it does not truncate the string after
    # the first '\0'.  However, such strings should never occur in
    # the first place.
    raw <- raw[raw != as.raw(0)];
    rawToChar(raw);
  }

  readByte <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=1, signed=TRUE, endian="big", n=n);
  }

  readUByte <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=1, signed=FALSE, endian="big", n=n);
  }

  readInt <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=4, signed=TRUE, endian="big", n=n);
  }

  readUInt <- function(con, n=1, ...) {
    # For 4-byte integers 'signed' can not be FALSE for readBin()
    readBin(con, what=integer(), size=4, signed=FALSE, endian="big", n=n);
  }

  readString <- function(con, ...) {
    nchars <- readInt(con);
    if (nchars == 0)
      return("");
    readChar(con, nchars=nchars);
  }

  readWString <- function(con, ...) {
    nchars <- readInt(con);
    if (nchars == 0)
      return("");
    raw <- readBin(con, what=raw(), n=2*nchars);
    raw <- raw[seq(from=2, to=length(raw), by=2)];
    rawToString(raw);
  }

  readRaw <- function(con, ...) {
    n <- readInt(con);
    if (n == 0)
      return(raw(0));
    readBin(con, what=raw(0), n=n);
  }

  readWVT <- function(con, ...) {
    name <- readWString(con);
    raw <- readRaw(con);
    type <- readWString(con);

    # Update data types
    # * text/x-calvin-integer-8
    # * text/x-calvin-unsigned-integer-8
    # * text/x-calvin-integer-16
    # * text/x-calvin-unsigned-integer-16
    # * text/x-calvin-integer-32
    # * text/x-calvin-unsigned-integer-32
    # * text/x-calvin-float
    # * text/plain
    n <- length(raw);
    value <- switch(type, 
      "text/ascii" = {
        rawToString(raw);
      },

      "text/plain" = {
        # Unicode/UTF-16?!?
        raw <- matrix(raw, ncol=2, byrow=TRUE);
        raw <- raw[,2];
        rawToString(raw);
      },

      "text/x-calvin-integer-8" = {
        readBin(raw, what=integer(0), endian="big", size=1, signed=TRUE, n=n);
      },

      "text/x-calvin-unsigned-integer-8" = {
        readBin(raw, what=integer(0), endian="big", size=1, signed=FALSE, n=n);
      },

      "text/x-calvin-integer-16" = {
        readBin(raw, what=integer(0), endian="big", size=2, signed=TRUE, n=n/2);
      },

      "text/x-calvin-unsigned-integer-16" = {
        readBin(raw, what=integer(0), endian="big", size=2, signed=FALSE, n=n/2);
      },

      "text/x-calvin-integer-32" = {
        readBin(raw, what=integer(0), endian="big", size=4, signed=TRUE, n=n/4);
      },

      "text/x-calvin-unsigned-integer-32" = {
        # For 4-byte integers 'signed' can not be FALSE for readBin()
        readBin(raw, what=integer(0), endian="big", size=4, signed=FALSE, n=n/4);
      },

      "text/x-calvin-float" = {
        readBin(raw, what=double(0), endian="big", size=4, n=n/4);
      },

      {
        raw;
      }
    ) # switch()

    list(name=name, value=value, raw=raw, type=type);
  } # readWVT()

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  hasFilter <- FALSE;
  if (!is.null(.filter)) {
    hasFilter <- TRUE;
  }

  # Nothing to do?
  if (hasFilter) {
    if (identical(.filter, FALSE) || length(.filter) == 0)
      return(NULL);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  hdr <- list(
    dataTypeId = readString(con),
    fileId = readString(con),
    timestamp = readWString(con),
    locale = readWString(con)
  )

  # Reading parameters
  nbrOfParams <- readInt(con);
  params <- vector("list", nbrOfParams);
  names <- character(nbrOfParams);
  for (kk in seq(length=nbrOfParams)) {
    wvt <- readWVT(con);
    names[kk] <- wvt$name;
    value <- wvt$value;
#    attr(value, "raw") <- wvt$raw;
    attr(value, "mimeType") <- wvt$type;
    params[[kk]] <- value;
  }
  names(params) <- names;
  hdr$parameters <- params;

  # Reading parent headers
  nbrOfParents <- readInt(con);
  parents <- vector("list", nbrOfParents);
  for (kk in seq(length=nbrOfParents)) {
    parents[[kk]] <- .readCcgDataHeader(con);
  }
  hdr$parents <- parents;
  
  hdr;
} # .readCcgDataHeader()




############################################################################
# HISTORY:
# 2012-05-18
# o Now using stop() instead of throw().
# 2011-11-01
# o CLEANUP: Changed signed=FALSE to signed=TRUE for readBin() calls
#   reading 4-byte integers in internal .readCcgFileHeader() and
#   .readCcgDataHeader().
# 2009-02-10
# o Added internal rawToString() replacing rawToChar() to avoid warnings
#   on "truncating string with embedded nul".
# 2007-08-16
# o Now the read data is converted according to the mime type.  See internal
#   readWVT() function.  The code is still ad hoc, so it is not generic.
#   For instance, it basically assumes that Unicode strings only contain
#   ASCII/ASCII-8 characters.
# 2006-11-06
# o Tested on Test3-1-121502.calvin.CEL and Test3-1-121502.calvin.CDF.
# o Created.  
############################################################################  

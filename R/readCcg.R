#########################################################################/**
# @RdocFunction readCcg
#
# @title "Reads an Affymetrix Command Console Generic (CCG) Data file"
#
# @synopsis
#
# \description{
#   @get "title".  The CCG data file format is also known as the
#   Calvin file format.
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
#  \details{
#    Note, the current implementation of this methods does not utilize the
#    Affymetrix Fusion SDK library.  Instead, it is implemented in R from the
#    file format definition [1].
#  }
#
# \section{About the CCG file format}{
#  A CCG file, consists of a "file header", a "generic data header",
#  and "data" section, as outlined here:
#  \itemize{
#   \item File Header
#   \item Generic Data Header (for the file)
#    \enumerate{
#     \item Generic Data Header (for the files 1st parent)
#      \enumerate{
#       \item Generic Data Header (for the files 1st parents 1st parent)
#       \item Generic Data Header (for the files 1st parents 2nd parent)
#       \item ...
#       \item Generic Data Header (for the files 1st parents Mth parent)
#      }
#    \item Generic Data Header (for the files 2nd parent)
#    \item ...
#    \item Generic Data Header (for the files Nth parent)
#   }
#   \item Data
#    \enumerate{
#     \item Data Group \#1
#      \enumerate{
#       \item Data Set \#1
#        \itemize{
#         \item Parameters
#         \item Column definitions
#         \item Matrix of data
#        }
#       \item Data Set \#2
#       \item ...
#       \item Data Set \#L
#      }
#     \item Data Group \#2
#     \item ...
#     \item Data Group \#K
#    }
#  }
# }
#
# @author "HB"
#
#  \seealso{
#    @see "readCcgHeader".
#    @see "readCdfUnits".
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
readCcg <- function(pathname, verbose=0, .filter=NULL, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument '.filter':
  hasFilter <- FALSE;
  if (!is.null(.filter)) {
    if (!is.list(.filter)) {
      stop("Argument '.filter' must be a list: ", mode(.filter));
    }
    hasFilter <- TRUE;
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Open file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  con <- file(pathname, open="rb");
  on.exit(close(con));

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Allocate return structure
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  ccg <- list();

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read file header
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  fhdr <- .readCcgFileHeader(con);
  if (hasFilter) {
    if (!identical(.filter$header, FALSE))
      ccg$fileHeader <- fhdr;
  } else {
    ccg$fileHeader <- fhdr;
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read the data header
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  ccg$genericDataHeader <- .readCcgDataHeader(con, .filter=.filter$dataHeader);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read the data
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  dataGroups <- .readCcgDataGroups(con, .filter=.filter$data, .fileHeader=fhdr);
  if (hasFilter) {
    if (!identical(.filter$dataGroups, FALSE))
      ccg$dataGroups <- dataGroups;
  } else {
    ccg$dataGroups <- dataGroups;
  }

  ccg;
} # readCcg()



.readCcgDataGroups <- function(pathname, .filter=NULL, .fileHeader=NULL, ...) {
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


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read file header?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (is.null(.fileHeader)) {
    .fileHeader <- .readCcgFileHeader(con);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read data groups
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  currFilter <- .filter;
  nextDataGroupStart <- .fileHeader$dataGroupStart;
  dataGroups <- list();
  for (gg in seq(length=.fileHeader$nbrOfDataGroups)) {
    dataGroupHeader <- .readCcgDataGroupHeader(con,
                                          fileOffset=nextDataGroupStart);
    # Next data group
    nextDataGroupStart <- dataGroupHeader$nextGroupStart;

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Apply filter
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     if (!is.null(.filter)) {
#       currFilter <- NULL;
#       if (is.null(names(.filter))) {
#         currFilter <- .filter[[gg]];
#       } else {
#         pos <- match(dataGroupHeader$name, names(.filter));
#         if (length(pos) > 0)
#           currFilter <- .filter[[pos]];
#       }
#     }
#     str(currFilter);

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Read data sets
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    offset <- dataGroupHeader$dataSetStart;
    dss <- vector("list", dataGroupHeader$nbrOfDataSets);
    names <- character(dataGroupHeader$nbrOfDataSets);
    for (kk in seq(along=dss)) {
      ds <- .readCcgDataSet(con, fileOffset=offset);

      offset <- ds$nextDataSetStart;
      dss[[kk]] <- ds;
      names[kk] <- ds$name;
    };
    names(dss) <- names;

    dataGroup <- list(
      header = dataGroupHeader,
      dataSets = dss
    );
    dataGroups <- c(dataGroups, list(dataGroup));
  } # while (nextDataGroupStart != 0)
  names(dataGroups) <- unlist(lapply(dataGroups, FUN=function(dg) {
    dg$header$name
  }), use.names=FALSE);

  dataGroups;
} # .readCcgDataGroups()



.readCcgDataGroupHeader <- function(con, fileOffset=NULL, ...) {
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

  readInt <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=4, signed=TRUE, endian="big", n=n);
  }

  readUInt <- function(con, n=1, ...) {
    # For 4-byte integers 'signed' can not be FALSE for readBin()
    readBin(con, what=integer(), size=4, signed=TRUE, endian="big", n=n);
  }

  readWString <- function(con, ...) {
    nchars <- readInt(con);
    if (nchars == 0)
      return("");
    bfr <- readBin(con, what=raw(), n=2*nchars);
    bfr <- bfr[seq(from=2, to=length(bfr), by=2)];
    rawToString(bfr);
  }

  readRaw <- function(con, ...) {
    n <- readInt(con);
    if (n == 0)
      return(raw(0));
    readBin(con, what=raw(0), n=n);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  #
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (!is.null(fileOffset)) {
    seek(con=con, where=fileOffset, offset="start", rw="read");
  }

  # Data Group
  # This section describes the data group. A data group is a group
  # of data sets. The file supports one or more data groups in a file.
  #
  # Item 	Description 	Type
  # 1 	File position of the next data group. When this is the last
  #     data group in the file, the value should be 0. 	UINT
  # 2 	File position of the first data set within the data group. 	UINT
  # 3 	The number of data sets within the data group. 	INT
  # 4 	The data group name. 	WSTRING
    nextGroupStart=readUInt(con)
    dataSetStart=readUInt(con)
    nbrOfDataSets=readInt(con)
    name=readWString(con)

  dataGroupHeader <- list(
    nextGroupStart=nextGroupStart,
    dataSetStart=dataSetStart,
    nbrOfDataSets=nbrOfDataSets,
    name=name
  )

  dataGroupHeader;
} # .readCcgDataGroupHeader()




.readCcgDataSet <- function(con, fileOffset=NULL, ...) {
  # Value Types
  # The following table defines the numeric values for the value types.
  # The value type is used to representing the type of value stored in
  # the file.
  #
  # Value 	Type
  # 0 	BYTE
  # 1 	UBYTE
  # 2 	SHORT
  # 3 	USHORT
  # 4 	INT
  # 5 	UINT
  # 6 	FLOAT
  # 7 	STRING
  # 8 	WSTRING
  whats <- c("integer", "integer", "integer", "integer", "integer",
            "integer", "double", "character", "character");
  names(whats) <- c("BYTE", "UBYTE", "SHORT", "USHORT", "INT", "UINT", "FLOAT", "STRING", "WSTRING");
  signeds <- c(TRUE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE);
  sizes <- c(1, 1, 2, 2, 4, 4, 4, 1, 2);

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

  readInt <- function(con, n=1, ...) {
    readBin(con, what=integer(), size=4, signed=TRUE, endian="big", n=n);
  }

  readUInt <- function(con, n=1, ...) {
    # For 4-byte integers 'signed' can not be FALSE for readBin()
    readBin(con, what=integer(), size=4, signed=TRUE, endian="big", n=n);
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
    bfr <- readBin(con, what=raw(), n=2*nchars);
    bfr <- bfr[seq(from=2, to=length(bfr), by=2)];
    rawToString(bfr);
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

#    cat(sprintf("Reading n=%d records of type '%s' named '%s'.\n", n, type, name));

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
        readBin(raw, what=integer(0), endian="big", size=2, signed=TRUE, n=n);
      },

      "text/x-calvin-unsigned-integer-16" = {
        readBin(raw, what=integer(0), endian="big", size=2, signed=FALSE, n=n);
      },

      "text/x-calvin-integer-32" = {
        readBin(raw, what=integer(0), endian="big", size=4, signed=TRUE, n=n);
      },

      "text/x-calvin-unsigned-integer-32" = {
        # For 4-byte integers 'signed' can not be FALSE for readBin()
        readBin(raw, what=integer(0), endian="big", size=4, signed=TRUE, n=n);
      },

      "text/x-calvin-float" = {
        readBin(raw, what=double(0), endian="big", size=4, n=n);
      },

      {
        raw;
      }
    ) # switch()

    list(name=name, value=value, raw=raw, type=type);
  } # readWVT()

  readWBI <- function(con, ...) {
    list(name=readWString(con), type=readByte(con), size=readInt(con));
  }


  if (!is.null(fileOffset)) {
    seek(con=con, where=fileOffset, offset="start", rw="read");
  }

#  Data Set
#  This section describes the data for a single data set item
#  (probe set, sequence, allele, etc.). The file supports one
#  or more data sets within a data group.
#
#  Item 	Description 	Type
#  1 	The file position of the first data element in the data set.
#     This is the first byte after the data set header. 	UINT
#  2 	The file position of the next data set within the data group.
#     When this is the last data set in the data group the value
#     shall be 1 byte past the end of the data set. This way the size
#     of the data set may be determined. 	UINT
#  3 	The data set name. 	WSTRING
#  4 	The number of name/value/type parameters. 	INT
#  5 	Array of name/value/type parameters. 	(WSTRING / VALUE / TYPE) [ ]
#  6 	Number of columns in the data set.
#     Example: For expression arrays, columns may include signal, p-value,
#     detection call and for genotyping arrays columns may include allele
#     call, and confidence value. For universal arrays, columns may
#     include probe set intensities and background. 	UINT
#  7 	An array of column names, column value types and column type sizes
#     (one per column).
#     The value type shall be represented by the value from the value type
#     table. The size shall be the size of the type in bytes. For strings,
#     this value shall be the size of the string in bytes plus 4 bytes for
#     the string length written before the string in the file.
#     (WSTRING / BYTE / INT) [ ]
#  8 	The number of rows in the data set. 	UINT
#  9 	The data set table, consisting of rows of columns (data values).
#     The specific type and size of each column is described by the data
#     and size types above. 	ROW [ ]
  dataSet <- list(
    elementsStart=readUInt(con),
    nextDataSetStart=readUInt(con),
    name=readWString(con)
  )
  # Reading parameters
  nbrOfParams <- readInt(con);
  params <- vector("list", nbrOfParams);
  names <- character(nbrOfParams);
  for (kk in seq(length=nbrOfParams)) {
    wvt <- readWVT(con);
    names[kk] <- wvt$name;
    value <- wvt$value;
    attr(value, "mimeType") <- wvt$type;
    params[[kk]] <- value;
  }
  names(params) <- names;
  dataSet$parameters <- params;

  # Reading columns
  nbrOfColumns <- readUInt(con);
  columns <- vector("list", nbrOfColumns);
  names <- character(nbrOfColumns);
  colWhats <- vector("list", nbrOfColumns);
  bytesPerRow <- 0;
  for (cc in seq(length=nbrOfColumns)) {
    wbi <- readWBI(con);
    names[cc] <- wbi$name;
    what <- whats[wbi$type+1];
    signed <- signeds[wbi$type+1];
    size <- wbi$size;
    bytesPerRow <- bytesPerRow + size;
    attr(what, "name") <- names(whats)[wbi$type+1];
    attr(what, "signed") <- signed;
    attr(what, "size") <- size;
    colWhats[[cc]] <- what;
  }
  names(colWhats) <- names;
  bytesPerRow <- as.integer(bytesPerRow);

  nbrOfRows <- readUInt(con);
  totalNbrOfBytes <- nbrOfRows * bytesPerRow;

  # Skip to the first element
  seek(con, which=dataSet$elementsStart, offset="start", rw="read");
  # Read all data row by row
  raw <- readBin(con, what=raw(), n=totalNbrOfBytes);
  dim(raw) <- c(bytesPerRow, nbrOfRows);

  table <- vector("list", nbrOfColumns);
  colsOffset <- 0;
  for (cc in seq(length=nbrOfColumns)) {
    what <- colWhats[[cc]];
    signed <- attr(what, "signed");
    size <- attr(what, "size");

    if (what == "character") {
      value <- matrix(raw[1:4,], nrow=nbrOfRows, ncol=4);
      raw <- raw[-c(1:4),,drop=FALSE];

      # Get the number of characters per string (all equal)
##      nchars <- readInt(con=value, n=nbrOfRows);
##      nchars <- nchars[1];
      nchars <- readInt(con=value, n=1);
      value <- NULL; # Not needed anymore

      ccs <- 1:(size-4);
      value <- raw[ccs,];
      raw <- raw[-ccs,,drop=FALSE];
      value <- rawToChar(value, multiple=TRUE);
      dim(value) <- c(nchars, nbrOfRows);

      # Build strings using vectorization (not apply()!)
      strs <- NULL;
      for (pp in seq(length=nrow(value))) {
        valuePP <- value[1,,drop=FALSE];
        value <- value[-1,,drop=FALSE];
        if (pp == 1) {
          strs <- valuePP;
        } else {
          strs <- paste(strs, valuePP, sep="");
        }
        valuePP <- NULL; # Not needed anymore
      }
      value <- strs;
      strs <- NULL; # Not needed anymore
    } else {
      ccs <- 1:size;
      value <- raw[ccs,,drop=FALSE];
      raw <- raw[-ccs,,drop=FALSE];
      value <- readBin(con=value, what=what, size=size, signed=signed, endian="big", n=nbrOfRows);
    }

    table[[cc]] <- value;
    colsOffset <- colsOffset + size;
  } # for (cc ...)

  # Turn into a data frame
  attr(table, "row.names") <- .set_row_names(length(table[[1]]));
  attr(table, "names") <- names;
  class(table) <- "data.frame";

  dataSet$table <- table;

  dataSet;
} # .readCcgDataSet()


############################################################################
# HISTORY:
# 2012-05-18
# o Now using stop() instead of throw().
# 2011-11-01
# o CLEANUP: Changed signed=FALSE to signed=TRUE for readBin() calls
#   reading 4-byte integers in internal .readCcgDataGroupHeader() and
#   .readCcgDataSet().
# 2009-02-10
# o Added internal rawToString() replacing rawToChar() to avoid warnings
#   on "truncating string with embedded nul".
# 2008-08-23
# o SPEED UP: Removed all gc() calls.
# 2008-01-13
# o Removed dependency on intToChar() in R.utils.
# o BUG FIX/UPDATE: The file format was updated between April 2006 and
#   November 2007.  More specifically, the so called "Value Types" were
#   changed/corrected.  Before values 7:9 were 'DOUBLE', 'STRING', and
#   'WSTRING'.  Now 7:8 are 'STRING' and 'WSTRING' and there is no longer
#   a 'DOUBLE'.
#   This was detected while trying to read a CNCHP file outputted by the
#   new Affymetrix Genotyping Console 2.0.  We can now read these files.
# 2007-08-16
# o Now it is only readCcg() and readCcgHeader() that are public.  The
#   other readCcgNnn() functions are renamed to .readCcgNnn().
# o Now the read data is converted according to the mime type.  See internal
#   readWVT() function.  The code is still ad hoc, so it is not generic.
#   For instance, it basically assumes that Unicode strings only contain
#   ASCII/ASCII-8 characters.
# 2006-11-06
# o Tested on Test3-1-121502.calvin.CEL and Test3-1-121502.calvin.CDF.
# o Created.
############################################################################

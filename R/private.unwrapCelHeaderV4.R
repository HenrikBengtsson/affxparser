.unwrapTagValuePairs <- function(bfr, ...) {
  trim <- function(s) {
    s <- gsub("^ *", "", s);
    s <- gsub(" *$", "", s);
  }

  bfr <- trim(bfr);
  patterns <- c("^([^:]*):([^;]*)[;]*(.*)$", "^([^=]*)=([^ ]*)[ ]*(.*)$");
  tags <- values <- c();
  while (nchar(bfr) > 0) {
    for (pattern in patterns) {
      tag <- gsub(pattern, "\\1", bfr);
      if (!identical(tag, bfr))
        break;
    }
    value <- gsub(pattern, "\\2", bfr);

    tags <- c(tags, tag);
    values <- c(values, value);

    bfr <- gsub(pattern, "\\3", bfr);
    bfr <- trim(bfr);
  }

  params <- as.list(values);
  names(params) <- tags;
  params;
}

.unwrapDatHeaderString <- function(header, ...) {
  trim <- function(s) {
    s <- gsub("^ *", "", s);
    s <- gsub(" *$", "", s);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Already a list?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (is.list(header)) {
  } else {
    header <- strsplit(header, split="\n")[[1]];
  }


  # Extract the "head" and the "tail" of the DAT header
  pattern <- "([^\024]*)(\024.*)";
  head <- gsub(pattern, "\\1", header);
  tail <- gsub(pattern, "\\2", header);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # [123456789012345678900123456789001234567890]
  # "[5..65534]  NA06985_H_tH_B5_3005533:",      ????
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  pattern <- "^([^:]*):(.*)$";
  if (regexpr(pattern, head) != -1) {
    bfr <- gsub(pattern, "\\1", header);
    header2 <- gsub(pattern, "\\2", header);
    bfr <- trim(bfr);             # Example: "[12..40151]  Fetal 3"
    if (nchar(bfr) > 0) {
      pattern <- "^([^ ]*])[ ]*(.*)[ ]*";
      pixelRange <- gsub(pattern, "\\1", bfr);
      sampleName <- gsub(pattern, "\\2", bfr);
      if (identical(pixelRange, sampleName)) {
        stop("Internal error: Failed to extract 'pixelRange' and 'sampleName' from DAT header.  They became identical: ", pixelRange);
      }
    } else {
      pixelRange <- "";
      sampleName <- "";
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Parse the DAT header
    #
    # 1. Number of pixels per row (padded with spaces), preceded with 
    #    "CLS=". char[9]
    # 2. Number of rows in the image (padded with spaces), preceded with 
    #    "RWS=".char[9]
    # 3. Pixel width in micrometers (padded with spaces), preceded with 
    #    "XIN=" char[7]
    # 4. Pixel height in micrometers (padded with spaces), preceded with 
    #    "YIN=". char[7]
    # 5. Scan speed in millimeters per second (padded with spaces), preceded 
    #    with "VE=". char[6]
    # 6. Temperature in degrees Celsius (padded with spaces). If no temperature
    #    was set then the entire field is empty. char[7]
    # 7. Laser power in milliwatts or microwatts (padded with spaces). char[4]
    # 8. Date and time of scan (padded with spaces). char[18]
    #
    # Example:
    # [123456789012345678900123456789001234567890] (See above)
    # "CLS=8714 ",
    # "RWS=8714 ",
    # "XIN=1  ",
    # "YIN=1  ",
    # "VE=30 ",
    # "       ",
    # "2.0 ",
    # "01/14/04 14:26:57 "
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    len <- c(9,9,7,7,6,7,4,18,220);
    ends <- cumsum(len);
    starts <- ends - len + 1;
    header <- substring(header2, starts, ends);
    header <- trim(header);
  
    # Store the last field
    bfr <- header[9];
  
    header <- list(
      pixelRange = pixelRange,
      sampleName = sampleName,
      CLS = gsub("^CLS=(.*)", "\\1", header[1]),
      RWS = gsub("^RWS=(.*)", "\\1", header[2]),
      XIN = gsub("^XIN=(.*)", "\\1", header[3]),
      YIN = gsub("^YIN=(.*)", "\\1", header[4]),
      VE = gsub("^VE=(.*)", "\\1", header[5]),
      scanTemp = header[6],
      laserPower = header[7],
      scanDate = header[8]
    );

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # The 'bfr' field:
    #
    # "There are several sub-fields in this field. The first sub field is the 
    #  scanner ID, sometimes followed by a number, followed by three spaces. 
    #  If the scanner ID is absent, the field consists of four spaces.
    #
    # Example:
    # [123456789012345678900123456789001234567890] (????)
    # "50101230  M10   \024  \024 Hind240.1sq \024  \024  \024  \024  
    # \024  \024  \024  \024  \024 6"
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # 0x14 == 024
    pattern <- "^([^\024]*)[ ]*(\024.*)$";
    scannerInfo <- gsub(pattern, "\\1", bfr);
    scannerInfo <- trim(scannerInfo);
    bfr <- gsub(pattern, "\\2", bfr);
  
  # Not locale safe: pattern <- "^([a-zA-Z0-9]*)[ ]*([a-zA-Z0-9]*)[ ]*";
    pattern <- "^([abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0-9]*)[ ]*([abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0-9]*)[ ]*";
    header$scanner <- list(
        id = gsub(pattern, "\\1", scannerInfo),
      type = gsub(pattern, "\\2", scannerInfo)
    );
  } else {
    # TO DO: Make these NAs to have the correct storage modes
    naValue <- as.character(NA);
    naValue <- "";
    header <- list(
      pixelRange = naValue,
      sampleName = naValue,
      CLS = naValue,
      RWS = naValue,
      XIN = naValue,
      YIN = naValue,
      VE = naValue,
      scanTemp = naValue,
      laserPower = naValue,
      scanDate = naValue,
      scanner = list(id=naValue, type=naValue)
    );
  }

  bfr <- tail;

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  #  Next are 10 structured comment fields. Each field is preceded by the 
  #  delimiter 0x14 and a space. The field is followed by a space and 0x14. 
  #  Only field two is valid, the other 9 fields are obsolete. Field 2 
  #  contains the probe array type, followed by .1sq. The 1sq extension is 
  #  also obsolete.
  #
  #  Next (after the last structured field) there is the chip orientation 
  #  preceded by a space.
  #
  #  The rest of the field is filled with nulls (zeros)".  Size: char[220]
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  pattern <- "\024 ([^\024]*)(.*)";
  values <- c();
  lastNchar <- -Inf;
  while (nchar(bfr) != lastNchar) {
    lastNchar <- nchar(bfr);
    value <- gsub(pattern, "\\1", bfr);
    value <- trim(value);
    bfr <- gsub(pattern, "\\2", bfr);
    values <- c(values, value);
  }

  header$misc <- values;
  header$chipType <- gsub("[.]1sq$", "", values[2]);

  header;
} # .unwrapDatHeaderString()



.unwrapCelHeaderV3String <- function(header, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Already a list?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (is.list(header)) {
  } else {
    header <- strsplit(header, split="\n")[[1]];
#    keep <- (unlist(lapply(header, FUN=nchar)) > 0);
#    header <- header[keep];
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Parse the CEL v3 header
  #
  # "The data in each section is of the format TAG=VALUE."
  #
  # Cols - The number of columns in the array (of cells).
  # Rows - The number of rows in the array (of cells).
  # TotalX - Same as Cols.
  # TotalY - Same as Rows.
  # OffsetX - Not used, always 0.
  # OffsetY - Not used, always 0.
  # GridCornerUL - XY coordinates of the upper left grid corner in pixel
  #   coordinates.
  # GridCornerUR - XY coordinates of the upper right grid corner in pixel
  #   coordinates.
  # GridCornerLR - XY coordinates of the lower right grid corner in pixel
  #   coordinates.
  # GridCornerLL - XY coordinates of the lower left grid corner in pixel
  #   coordinates.
  # Axis-invertX - Not used, always 0.
  # AxisInvertY - Not used, always 0.
  # swapXY - Not used, always 0.
  # DatHeader - The header from the DAT file.
  # Algorithm  - The algorithm name used to create the CEL file.
  # AlgorithmParameters - The parameters used by the algorithm. The format 
  #   is TAG:VALUE pairs separated by semi-colons or TAG=VALUE pairs separated
  #   by spaces.
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  pattern <- "^([-a-zA-Z0-9]*)=(.*)$";
  names <- gsub(pattern, "\\1", header);
  values <- gsub(pattern, "\\2", header);
  names(values) <- names;
  header <- as.list(values);
  # Fix some mishaps in names (sic!; see DevNet forum this week) /HB 2006-09-10
#  names(header) <- gsub("^Axis-invert", "Axis-Invert", names(header));
#  names(header) <- gsub("^AxisInvert", "Axis-Invert", names(header));

  # Assert that all mandatory fields are there
  knownFields <- c("Cols", "Rows", "TotalX", "TotalY", "OffsetX", "OffsetY", "GridCornerUL", "GridCornerUR", "GridCornerLR", "GridCornerLL", "Axis-invertX", "AxisInvertY", "swapXY", "DatHeader", "Algorithm", "AlgorithmParameters");
  missing <- !(knownFields %in% names(header));
  if (any(missing)) {
    stop("Argument 'header' does not contain all mandatory fields: ", 
                                   paste(knownFields[missing], collapse=", "));
  }

  # Unwrap DAT header string
  header$DatHeader <- .unwrapDatHeaderString(header$DatHeader);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Unwrap 'AlgorithmParameters':
  #
  # AlgorithmParameters - The parameters used by the algorithm. The format 
  #   is TAG:VALUE pairs separated by semi-colons or TAG=VALUE pairs separated
  #   by spaces.
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  header$AlgorithmParameters <- .unwrapTagValuePairs(header$AlgorithmParameters);

  header;
} # .unwrapCelHeaderV3String()



# \arguments{
#   \item{header}{A @list structure as returned by @see "readCelHeader".}
# }
.unwrapCelHeaderV4 <- function(header, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'header':
  if (!is.list(header)) {
    stop("Argument 'header' is not a list: ", mode(header));
  }

  # Assert that all header fields are there
  knownFields <- c("version", "cols", "rows", "total", "algorithm", "parameters", "chiptype", "header", "cellmargin", "noutliers", "nmasked");
  missing <- !(knownFields %in% names(header));
  if (any(missing)) {
    stop("Argument 'header' does not contain all mandatory fields: ", 
                                   paste(knownFields[missing], collapse=", "));
  }

  header$parameters <- .unwrapTagValuePairs(header$parameters);

  header$header <- .unwrapCelHeaderV3String(header$header);

  header;
} # .unwrapCelHeaderV4()



############################################################################
# HISTORY:
# 2011-02-22
# o ROBUSTNESS/BUG FIX: The internal .unwrapDatHeaderString() would
#   throw "Internal error: Failed to extract 'pixelRange' and 'sampleName' 
#   from DAT header.  They became identical: ..." in case the DAT header
#   of the CEL file did not contain all fields.  The function has now
#   been updated to be more forgiving and robust so that missing values
#   are returned for such fields instead.
# 2007-08-16
# o BUG FIX: Internal .unwrapDatHeaderString() failed to correctly extract
#   'pixelRange' and 'sampleName' from DAT header.
# 2006-12-28
# o R CMD check v2.5.0 devel complained about: Warning: '\]' is an 
#   unrecognized escape in a character string. Warning: unrecognized escape
#   removed from "^([^\]]*])[ ]*(.*)[ ]*".  Replaced with '\\]'.
# 2006-09-10
# o BUG FIX: Local trim() was missing in one of the private functions.
# 2006-09-06
# o Created.  This is used by writeCelHeaderV4().
############################################################################

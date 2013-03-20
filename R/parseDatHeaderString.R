########################################################################/**
# @RdocFunction parseDatHeaderString
#
# @title "Parses a DAT header string"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{header}{A @character string.}
#  \item{timeFormat}{The format string used to parse the timestamp.
#    For more details, see \code{\link[base:strptime]{strptime}()}.
#    If @NULL, no parsing is done.}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns named @list structure.
# }
#
# \seealso{
#  @see "readCelHeader".
# }
#
# @author "HB"
#
# @keyword programming
# @keyword file
# @keyword IO
# @keyword internal
#**/####################################################################### 
parseDatHeaderString <- function(header, timeFormat="%m/%d/%y %H:%M:%S", ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  trim <- function(s, ...) {
    s <- as.character(s);
    s <- sub("^[\t\n\f\r ]*", "", s);
    s <- sub("[\t\n\f\r ]*$", "", s);
    s;
  } # trim();


  # <quote>
  #   There are several sub-fields in this field. The first sub field is the
  # scanner ID, sometimes followed by a number, followed by three spaces.
  # If the scanner ID is absent, the field consists of four spaces.
  #   Next are 10 structured comment fields. Each field is preceded by the
  # delimiter 0x14 and a space. The field is followed by a space and 0x14.
  # Only field two is valid, the other 9 fields are obsolete. Field 2 
  # contains the probe array type, followed by .1sq. The 1sq extension is
  # also obsolete.
  #   Next (after the last structured field) there is the chip orientation
  # preceded by a space.
  #   The rest of the field is filled with nulls (zeros)
  # </quote>

  # Split by ASCII 0x14 delimiter
  parts <- strsplit(header, split=" \024 ", fixed=TRUE)[[1]];
  field <- parts[1];
  commentFields <- parts[-1];

  res <- list();
  pattern <- ".*CLS=(.{5})RWS=(.{5})XIN=(.{3})YIN=(.{3})VE=(.{3}).*(.{7})(.{4})";
  # Number of pixels per row (padded with spaces), preceded with "CLS=".
  res$nbrOfPixelColumns <- as.integer(gsub(pattern, "\\1", field));

  # Number of rows in the image (padded with spaces), preceded with "RWS=".
  res$nbrOfPixelRows <- as.integer(gsub(pattern, "\\2", field));

  # Pixel width in micrometers (padded with spaces), preceded with "XIN="
  res$pixelWidth <- as.double(gsub(pattern, "\\3", field));

  # Pixel height in micrometers (padded with spaces), preceded with "YIN="
  res$pixelHeight <- as.double(gsub(pattern, "\\4", field));

  # Scan speed in millimeters per second (padded with spaces), 
  # preceded with "VE=".
  res$scanSpeed <- as.double(gsub(pattern, "\\5", field));

  # Temperature in degrees Celsius (padded with spaces). If no temperature was
  # set then the entire field is empty.
  res$temperature <- as.double(gsub(pattern, "\\6", field));

  # Laser power in milliwatts or microwatts (padded with spaces).
  res$laserPower <- as.double(gsub(pattern, "\\6", field));


  # Find the element with a date. It is part of the same string as the
  # one containing the chip type.  Get the chip type from the header.
  # Extract the date timestamp
  pattern <- ".*([01][0-9]/[0-3][0-9]/[0-9][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]).*";
  timestamp <- gsub(pattern, "\\1", header);
  timestamp <- trim(timestamp); # Unnecessary?

  # Parse the identified timestamp into POSIXct?
  if (!is.null(timeFormat)) {
    timestamp <- strptime(timestamp, format=timeFormat, ...);
    # If no valid timestamp was found, return NA.
    if (length(as.character(timestamp)) == 0) {
      timestamp <- as.POSIXct(NA);
    }
  }
  res$timestamp <- timestamp;

  res$chipType <- trim(gsub("[.]1sq", "", commentFields[2]));

  res;
} # parseDatHeaderString()


############################################################################
# HISTORY:
# 2009-09-21
# o Created from internal code in aroma.affymetrix.
############################################################################

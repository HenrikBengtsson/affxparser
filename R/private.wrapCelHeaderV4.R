.wrapTagValuePairs <- function(args, ...) {
  fmtstr <- "%s=%s";
  params <- unlist(args);
  values <- sprintf(fmtstr, names(params), params);
  values <- paste(values, collapse=";")
} # .wrapTagValuePairs()


.wrapDatHeader <- function(header, ...) {
  bfr <- c();

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # [123456789012345678900123456789001234567890]
  # "[5..65534]  NA06985_H_tH_B5_3005533:",      ????
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  fmtstr <- "%s  %s:";
  value <- sprintf(fmtstr, header$pixelRange, header$sampleName);
  bfr <- c(bfr, value);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Warp the DAT header
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
  fmtstr <- "CLS=%-5.5sRWS=%-5.5sXIN=%-3.3sYIN=%-3.3sVE=%-3.3s%-7.7s%-4.4s%-18.18s";
  value <- sprintf(fmtstr, header$CLS, header$RWS, header$XIN, header$YIN, header$VE, header$scanTemp, header$laserPower, header$scanDate);

  # Assert correct length (9+9+7+7+6+7+4+18=67)
  if (nchar(value) != 67)
    stop("Internal error in .wrapDatHeader(). Incorrect string length (", nchar(value), " != 67): ", value);
  bfr <- c(bfr, value);
 
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  # [123456789012345678900123456789001234567890] (????)
  # "<scanner-id> <scanner-type>   "
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (nchar(header$scanner$id) == 0) {
    value <- "   ";
  } else {
    fmtstr <- "%s  %s   ";
    value <- sprintf(fmtstr, header$scanner$id, header$scanner$type);
  }
  bfr <- c(bfr, value);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  # [123456789012345678900123456789001234567890] (????)
  # "\024  \024 <chip-type> \024  \024  \024  \024  \024  \024  \024  \024  \024 "
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Make sure 'misc' is of length 10.
  header$misc <- c(header$misc, rep("", 20-length(header$misc)));
  header$misc <- header$misc[1:10];

  # IMPORTANT: Overwrite 'chip type' value
  if (is.null(header$chipType))
    stop("DAT header has not 'chipType' field.");
  header$misc[2] <- sprintf("%s.1sq", header$chipType);

  fmtstr <- "\024 %s ";
  values <- sprintf(fmtstr, header$misc);
  values <- paste(values, collapse="");
#  values <- paste(values, "\024 6", sep="");  
  bfr <- c(bfr, values);

  bfr <- paste(bfr, collapse="");
  bfr;
} # .wrapDatHeader()



.wrapCelHeaderV3 <- function(header, ...) {
 # Make sure the header is consistent
  header$TotalX <- header$Cols;
  header$TotalY <- header$Rows;
  header$OffsetX <- 0;
  header$OffsetY <- 0;
  header$"Axis-invertX" <- 0;
  header$"AxisInvertY" <- 0;
  header$swapXY <- 0;

  # Wrap up the DAT header
  header$DatHeader <- .wrapDatHeader(header$DatHeader);

  # Wrap up the 'AlgorithmParameters' header
  header$AlgorithmParameters <- .wrapTagValuePairs(header$AlgorithmParameters);

  # Wrap up everything else
  fmtstr <- "%s=%s";
  header <- unlist(header);
  header <- sprintf(fmtstr, names(header), header);
  header <- paste(header, collapse="\n")
  header <- paste(header, "\n", sep="");
  
  header;
} # .wrapCelHeaderV3()


.wrapCelHeaderV4 <- function(header, ...) {
  # Make sure the fields are consistent
  header$version <- as.integer(4);
  header$total <- header$cols * header$rows;

  # Make sure the CEL V3 header is consistent
  headerV3 <- header$header; 

  headerV3$Cols <- header$cols;
  headerV3$Rows <- header$rows;

  # Override any algorithm and parameters in V3 header
  headerV3$Algorithm <- header$algorithm;
  headerV3$AlgorithmParameters <- header$parameters;

  headerV3 <- .wrapCelHeaderV3(headerV3);
  header$header <- headerV3;

  # Not needed anymore, wrap them up
  header$parameters <- .wrapTagValuePairs(header$parameters);

  header;
} # .wrapCelHeaderV4()


############################################################################
# HISTORY:
# 2007-08-16
# o Now internal .wrapCelHeaderV4() sets the version number as an integer.
# 2006-09-06
# o Created.  This is used by writeCelHeaderV4().
############################################################################

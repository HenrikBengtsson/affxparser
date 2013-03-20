########################################################################/-Rdoc TURNED OFF-**
# @RdocFunction .readCelHeaderV3
#
# @title "Read the header of a CEL v3 (ASCII) file"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{con}{An open and readable @connection.}
# }
#
# \value{
#  Returns a named @list structure.
# }
#
# \details{
#  When the header is read, the file pointer is at the line after the
#  header.  Empty lines should be expected before the intensity section.
#  This function does not make use of Fusion SDK.
# }
#
# @author "HB"
#
# @keyword IO
# @keyword File
# @keyword internal
#**-Rdoc TURNED OFF-/#######################################################################
.readCelHeaderV3 <- function(con, ...) {
  trim <- function(s, ...) {
    s <- gsub("^[ \t]*", "", s);
    s <- gsub("[ \t]*$", "", s);
    s;
  }

  # Read "[CEL]"
  while (TRUE) {
    tag <- trim(readLines(con, n=1));
    if (!identical(tag, ""))
      break;
  }
  if (!identical(tag, "[CEL]")) {
    stop("Could not read CEL v3 file header. File format error: File does not start with [CEL]: ", tag);
  }

  # Read version    
  version <- trim(readLines(con, n=1));
  if (!identical(version, "Version=3")) {
    stop("Could not read CEL v3 file header. Not a version 3 file: ", version);
  }

  # Read "[HEADER]"
  while (TRUE) {
    tag <- trim(readLines(con, n=1));
    if (!identical(tag, ""))
      break;
  }
  if (!identical(tag, "[HEADER]")) {
    stop("Could not read CEL v3 file header. Expected HEADER section, but got: ", tag);
  }

  # Read header fields
  header <- list(
    version = as.integer(3)
  );
  while (TRUE) {
    field <- trim(readLines(con, n=1));
    if (identical(field, ""))
      break;
    field <- unlist(strsplit(field, split="="));
    key <- field[1];
    value <- paste(field[-1], collapse="=");
    header[[key]] <- value;
  }

  # Fields to be converted to integers
  intFields <- c("Cols", "Rows", "TotalX", "TotalY", 
                 "OffsetX", "OffsetY", "swapXY");
  # Ad hoc, but the "Axis" fields are sometimes misspelled.
  axisFields <- grep("^Axis[-]*(i|I)nvert(X|Y)$", names(header), value=TRUE);
  intFields <- c(intFields, axisFields);
  for (ff in intersect(names(header), intFields)) {
    header[[ff]] <- as.integer(header[[ff]]);
  }

  # Vector fields 
  for (ff in grep("GridCorner", names(header))) {
    value <- header[[ff]];
    value <- unlist(strsplit(value, split=" "));
    value <- trim(value);
    value <- as.integer(value);
    header[[ff]] <- value;
  }

  header;
} # .readCelHeaderV3()


############################################################################
# HISTORY:
# 2006-07-10
# o Created from .readCelHeaderV3().
############################################################################  

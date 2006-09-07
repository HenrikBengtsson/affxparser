#########################################################################/**
# @RdocFunction convertCdf
#
# @title "Converts a CDF into the same CDF but with another format"
#
# @synopsis 
# 
# \description{
#   @get "title".  
#   Currently only CDF files in version 4 (binary/XDA) can be written.
#   However, any input format is recognized.
# }
# 
# \arguments{
#   \item{filename}{The pathname of the original CDF file.}
#   \item{outFilename}{The pathname of the destination CDF file.}
#   \item{version}{The version of the output file format.}
#   \item{force}{If @FALSE, and the version of the orignal CDF is the
#     same as the output version, the new CDF will not be generated,
#     otherwise it will.}
#   \item{...}{Not used.}
#   \item{.validate}{If @TRUE, a consistency test between the generated 
#     and the original CDF is performed.}
#   \item{verbose}{If @TRUE, extra details are written while processing.}
# }
# 
# \value{
#   Returns (invisibly) @TRUE if a new CDF was generated, otherwise @FALSE.
# }
#
# @examples "../incl/convertCdf.Rex"
#
# @author
#
# \seealso{
#   @see "writeCdf".
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
convertCdf <- function(filename, outFilename, version="4", force=FALSE, ..., .validate=TRUE, verbose=FALSE) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'filename':
  if (!file.exists(filename)) {
    stop("Cannot open CDF file. File does not exist: ", filename);
  }

  # Argument 'version':
  version <- as.character(version);
  if (version == "4") {
  } else {
    stop("Cannot convert CDF. Currently only version 4 (binary/XDA) can be written: ", version);
  }

  # Argument 'force':
  force <- as.logical(force);

  # Argument '.validate':
  .validate <- as.logical(.validate);

  # Argument 'verbose':
  verbose <- as.integer(verbose);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Open source CDF and check file-format versions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose)
    cat("Reading CDF header...\n");
  cdfHeader <- readCdfHeader(filename);
#  if (version == cdfHeader$version) {
#    if (!force) {
#      warning("The original CDF file is already in file-format version ", 
#                                                  version, ": ", filename);
#      if (verbose)
#        cat("Reading CDF header...done\n");
#      return(invisible(FALSE));
#    }
#  }
  if (verbose)
    cat("Reading CDF header...done\n");

  # Read QC units
  if (verbose)
    cat("Reading CDF QC units...\n");
  cdfQcUnits <- readCdfQc(filename)
  if (verbose)
    cat("Reading CDF QC units...done\n");
  
  # Reading units
  if (verbose)
    cat("Reading CDF units...\n");
  cdfUnits <- readCdf(filename)
  if (verbose)
    cat("Reading CDF units...done\n");

  if (verbose)
    cat("Writing CDF structure...\n");
  writeCdf(pathname, cdfheader=cdfHeader, cdf=cdfUnits, 
         cdfqc=cdfQcUnits, overwrite=TRUE, verbose=verbose-1);
  if (verbose)
    cat("Writing CDF structure...done\n");

  # Validate?
  if (.validate) {
    # CDF header
    value <- cdfHeader
    value2 <- readCdfHeader(pathname)
    keep <- setdiff(names(value), c("chiptype", "filename"))
    value <- value[keep]
    value2 <- value2[keep]
    if (!identical(value, value2)) {
      stop("Validation error. The header of the written CDF is not consistent with the original CDF.");
    }

    # QC units    
    value <- cdfQcUnits
    value2 <- readCdfQc(pathname)
    if (!identical(value, value2)) {
      stop("Validation error. The QC units of the written CDF is not consistent with the original CDF.");
    }

    # Units    
    value <- cdfUnits
    value2 <- readCdf(pathname)
    if (!identical(value, value2)) {
      stop("Validation error. The units of the written CDF is not consistent with the original CDF.");
    }
  } # if (.validate)

  invisible(TRUE);
} # convertCdf()


############################################################################
# HISTORY:
# 2006-09-07
# o Created.
############################################################################

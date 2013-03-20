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
#   \item{outFilename}{The pathname of the destination CDF file.
#     If the same as the source file, an exception is thrown.}
#   \item{version}{The version of the output file format.}
#   \item{force}{If @FALSE, and the version of the orignal CDF is the
#     same as the output version, the new CDF will not be generated,
#     otherwise it will.}
#   \item{...}{Not used.}
#   \item{.validate}{If @TRUE, a consistency test between the generated 
#     and the original CDF is performed.  Note that the memory overhead
#     for this can be quite large, because two complete CDF structures 
#     are kept in memory at the same time.}
#   \item{verbose}{If @TRUE, extra details are written while processing.}
# }
# 
# \value{
#   Returns (invisibly) @TRUE if a new CDF was generated, otherwise @FALSE.
# }
#
# \section{Benchmarking of ASCII and binary CDFs}{
#   Binary CDFs are much faster to read than ASCII CDFs.  Here are some
#   example for reading complete CDFs (the differnce is even larger when
#   reading CDFs in subsets):
#   \itemize{
#     \item HG-U133A (22283 units): ASCII 11.7s (9.3x), binary 1.20s (1x).
#     \item Hu6800 (7129 units): ASCII 3.5s (6.1x), binary 0.57s (1x).
#   }
# }
#
# \section{Confirmed convertions to binary (XDA) CDFs}{
#   The following chip types have been converted using \code{convertCdf()} 
#   and then verified for correctness using \code{compareCdfs()}:
#   ASCII-to-binary: HG-U133A, Hu6800.
#   Binary-to-binary: Test3.
# }
#
# @examples "../incl/convertCdf.Rex"
#
# @author "HB"
#
# \seealso{
#   See @see "compareCdfs" to compare two CDF files.
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
  # Expand any '~' in the pathname.
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename)) {
    stop("Cannot open CDF file. File does not exist: ", filename);
  }

  # Argument 'outFilename':
  # Expand any '~' in the pathname.
  outFilename <- file.path(dirname(outFilename), basename(outFilename));
  if (identical(outFilename, filename)) {
    stop("Cannot convert CDF file. Destination is identical the the source pathname: ", filename);
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
  # Read source CDF
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


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Write new CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose)
    cat("Writing CDF structure...\n");
  verbose2 <- verbose-1;
  if (verbose2 < 0) verbose2 <- 0;
t <- system.time({
  writeCdf(outFilename, cdfheader=cdfHeader, cdf=cdfUnits, 
         cdfqc=cdfQcUnits, overwrite=TRUE, verbose=verbose2);
}); 
  if (verbose) {
    cat("Timing for writeCdf():\n");
    print(t);
  }
  if (verbose)
    cat("Writing CDF structure...done\n");


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (.validate) {
    if (verbose)
      cat("Comparing CDFs...\n");
    res <- compareCdfs(filename, outFilename, verbose=verbose);
    if (!res) {
      stop("An inconsistency between source and destination CDF was detected. Reason: ", attr(res, "reason"));
    }
    if (verbose)
      cat("Comparing CDFs...done\n");
  } # if (.validate)

  invisible(TRUE);
} # convertCdf()


############################################################################
# HISTORY:
# 2007-07-26
# o Removed debug assignment 'res2 <<- res' before validation error message.
# 2006-09-09
# o More benchmarking: A binary-to-binary convertion of Mapping50K_Hind240
#   (56.0Mb; 57299 units) takes in total 25 mins.  It is the writing of 
#   units that is slow.
# o Simple benchmarking: On Thinkpad A31 1Gb RAM; HG-U133A.CDF (22283 units)
#   ASCII (40.3Mb) -> XDA (10.8Mb): ~15s to read ASCII, ~270s to write XDA,
#   and ~420s to validate.
# o Now validate is making use of compareCdfs().
# 2006-09-08
# o Added protection against overwriting the source file.
# 2006-09-07 /HB
# o Ken Simpson at WEHI just tried to convert an ASCII Exon CDF to a binary
#   CDF using convertCdf().  The ASCII CDF is ???MB, and the binary one 
#   became 321MB.  He did this on a 4-core with 16GB RAM Linux system, and
#   it took 45 minutes, which probably includes the validation tests,
#   which eventually made R run out of memory.  I've updated the code a bit
#   to make the validation a little bit more memory efficient.  It is 
#   probably better to compare chunks of units and not all of them at the
#   same time.  The writeCdf() function can be made faster if writing to
#   a raw vector internally which is the dumped to file.  This should be
#   done in chunks to also optimize memory usage.
# o Created.
############################################################################

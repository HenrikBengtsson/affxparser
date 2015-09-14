#########################################################################/**
# @RdocFunction convertCel
#
# @title "Converts a CEL into the same CEL but with another format"
#
# @synopsis
#
# \description{
#   @get "title".
#   Currently only CEL files in version 4 (binary/XDA) can be written.
#   However, any input format is recognized.
# }
#
# \arguments{
#   \item{filename}{The pathname of the original CEL file.}
#   \item{outFilename}{The pathname of the destination CEL file.
#     If the same as the source file, an exception is thrown.}
#   \item{readMap}{An optional read map for the input CEL file.}
#   \item{writeMap}{An optional write map for the output CEL file.}
#   \item{version}{The version of the output file format.}
#   \item{newChipType}{(Only for advanced users who fully understands
#      the Affymetrix CEL file format!)
#      An optional string for overriding the chip type (label)
#      in the CEL file header.}
#   \item{...}{Not used.}
#   \item{.validate}{If @TRUE, a consistency test between the generated
#     and the original CEL is performed.}
#   \item{verbose}{If @TRUE, extra details are written while processing.}
# }
#
# \value{
#   Returns (invisibly) @TRUE if a new CEL was generated, otherwise @FALSE.
# }
#
# \section{Benchmarking of ASCII and binary CELs}{
#   Binary CELs are much faster to read than ASCII CELs.  Here are some
#   example for reading complete CELs (the differnce is even larger when
#   reading CELs in subsets):
#   \itemize{
#     \item To do
#   }
# }
#
# \section{WARNING: Changing the chip type label}{
#   The \code{newChipType} argument changes the label in the
#   part of DAT header that specifies the chip type of the
#   CEL file.  Note that it does not change anything else in
#   the CEL file.  This type of relabelling is valid for updating
#   the chip type \emph{label} of CEL files that where generated
#   during, say, an "Early Access" period leading to a different
#   chip type label than what more recent CEL files of the same
#   physical chip type have.
# }
#
# @examples "../incl/convertCel.Rex"
#
# @author "HB"
#
# \seealso{
#   @see "createCel".
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
convertCel <- function(filename, outFilename, readMap=NULL, writeMap=NULL, version="4", newChipType=NULL, ..., .validate=FALSE, verbose=FALSE) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'filename':
  # Expand any '~' in the pathname.
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename)) {
    stop("Cannot open CEL file. File does not exist: ", filename);
  }

  # Argument 'outFilename':
  # Expand any '~' in the pathname.
  outFilename <- file.path(dirname(outFilename), basename(outFilename));
  if (identical(outFilename, filename)) {
    stop("Cannot convert CEL file. Destination is identical the the source pathname: ", filename);
  }
  if (file.exists(outFilename)) {
    stop("Cannot convert CEL file. Destination file already exists: ", outFilename);
  }

  # Argument 'version':
  version <- as.character(version);
  if (version == "4") {
  } else {
    stop("Cannot convert CEL. Currently only version 4 (binary/XDA) can be written: ", version);
  }

  # Argument 'newChipType':
  if (!is.null(newChipType)) {
    newChipType <- as.character(newChipType);
    if (nchar(newChipType) == 0) {
      stop("Argument 'newChipType' cannot be an empty string.");
    }
  }

  # Argument 'verbose':
  verbose <- as.integer(verbose);

  verbose2 <- verbose-1;
  if (verbose2 < 0) verbose2 <- 0;


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read source CEL
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose)
    cat("Reading CEL file...\n");
  cel <- readCel(filename, readHeader=TRUE, readXY=TRUE, readIntensities=TRUE, readStdvs=TRUE, readPixels=TRUE, readOutliers=FALSE, readMasked=FALSE, readMap=readMap);
  if (verbose)
    cat("Reading CEL file...done\n");


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Changing chip type?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  hdr <- cel$header;
  if (!is.null(newChipType)) {
    if (verbose) {
      cat("Updating the chip type label from '", hdr$chiptype, "' to '",
                                              newChipType, "'.\n", sep="");
    }

    # Updating v3 headers
    for (field in c("header", "datheader")) {
      header <- hdr[[field]]
      if (is.null(header)) next
      pattern <- sprintf("%s.1sq", hdr$chiptype)
      target <- sprintf("%s.1sq", newChipType)
      header <- gsub(pattern, target, header, fixed=TRUE)
      hdr[[field]] <- header
    }

    # Updating chip type field (this is actually read only, because
    # the chip type is always inferred from the v3 header).
    hdr$chiptype <- newChipType;

    pattern <- target <- header <- NULL; # Not needed anymore
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Creating new CEL file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose)
    cat("Creating empty CEL file...\n");
  suppressWarnings({
    # createCel() will generate a warning if the CDF file could not be
    # located, but that is all right.
    pathname <- createCel(outFilename, header=hdr, overwrite=FALSE, verbose=verbose2);
  });
  hdr <- NULL; # Not needed anymore

  if (verbose)
    cat("Creating empty CEL file...done\n");


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Update destination CEL file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (verbose)
    cat("Updating CEL file...\n");
  updateCel(outFilename, intensities=cel, verbose=verbose2, writeMap=writeMap);
  cel <- NULL; # Not needed anymore

  if (verbose)
    cat("Updating CEL file...done\n");

  if (.validate) {
    if (verbose)
      cat("Validating CEL file...\n");
    if (is.null(writeMap)) {
      otherReadMap <- NULL;
    } else {
      otherReadMap <- invertMap(writeMap);
    }

    compareCels(filename, outFilename, readMap=readMap,
                                otherReadMap=otherReadMap, verbose=verbose);
    if (verbose)
      cat("Validating CEL file...done\n");
  }

  invisible(pathname);
} # convertCel()


############################################################################
# HISTORY:
# 2009-02-20
# o Removed all gc() in convertCel().
# o Added optional argument 'newChipType' to convertCel() for overriding
#   the default chip type.
# 2007-09-12
# o Help page was refering to the "CDF" and not the "CEL" files.
# o Now convertCel() will not give a warning if the CDF file is not found.
# 2007-08-28
# o BUG FIX: If the destination file already existed, convertCel() would
#   correctly detect that, but would give the name of the source file.
# 2007-08-16
# o Now createCel() coerces the CEL header to version 4, so writeCel()
#   should be more generic.
# 2007-03-28
# o Memory optimization; removing non-needed objects asap and calls gc().
# o There was non-used variables in convertCel().
# 2007-01-04
# o Creates identical output according to validateCels().
# o Added 'readMap' and 'writeMap' arguments.
# 2007-01-03
# o Created.
############################################################################

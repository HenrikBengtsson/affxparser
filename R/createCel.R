#########################################################################/**
# @RdocFunction createCel
#
# @title "Creates an empty CEL file"
#
# @synopsis 
# 
# \description{
#   @get "title".
# }
# 
# \arguments{
#   \item{filename}{The filename of the CEL file to be created.}
#   \item{header}{A @list structure describing the CEL header, similar
#     to the structure returned by @see "readCelHeader".  This header
#     can be of any CEL header version.}
#   \item{overwrite}{If @FALSE and the file already exists, an exception
#     is thrown, otherwise the file is created.}
#   \item{nsubgrids}{The number of subgrids.}
#   \item{...}{Not used.}
#   \item{cdf}{(optional) The pathname of a CDF file for the CEL file
#     to be created.  If given, the CEL header (argument \code{header})
#     is validated against the CDF header, otherwise not.
#     If @TRUE, a CDF file is located automatically based using
#     \code{findCdf(header$chiptype)}.
#   }
#   \item{verbose}{An @integer specifying how much verbose details are
#     outputted.}
# }
# 
# \value{
#   Returns (invisibly) the pathname of the file created.
# }
#
# \details{
#   Currently only binary (v4) CEL files are supported.
#   The current version of the method does not make use of the Fusion SDK,
#   but its own code to create the CEL file.
# }
#
# \section{Redundant fields in the CEL header}{
#   There are a few redundant fields in the CEL header.  To make sure
#   the CEL header is consistent, redundant fields are cleared and 
#   regenerated.  For instance, the field for the total number of cells
#   is calculated from the number of cell rows and columns.
# }
#
# @examples "../incl/createCel.Rex"
#
# @author "HB"
# 
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
createCel <- function(filename, header, nsubgrids=0, overwrite=FALSE, ..., cdf=NULL, verbose=FALSE) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  writeInteger <- function(con, data, ...) {
    writeBin(con=con, as.integer(data), size=4, endian="little");
  }
  
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'overwrite':
  overwrite <- as.logical(overwrite);

  # Argument 'filename':
  if (file.exists(filename)) {
    if (!overwrite)
      stop("Cannot create CEL file. File already exists: ", filename)
  }

  # Argument 'header':
  if (!is.list(header)) {
    stop("Argument 'header' is not a list: ", mode(header));
  }

  # Argument 'nsubgrids':
  nsubgrids <- as.integer(nsubgrids);
  if (nsubgrids < 0) {
    stop("Argument 'nsubgrids' is negative: ", nsubgrids);
  }

  # Argument 'cdf':
  if (is.character(cdf)) {
    if (!file.exists(cdf)) {
      stop("Cannot compare to CDF file. File not found: ", cdf);
    }
  } else if (is.logical(cdf)) {
  } else if (!is.null(cdf)) {
    stop("Invalid type of argument 'cdf': ", mode(cdf));
  }




  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Infer CEL version
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  version <- .getCelHeaderVersion(header);
  if (version == "1") {
    if (verbose)
      cat("Coercing CEL header to v4...\n");
    header <- .getCelHeaderV4(header);
    if (verbose)
      cat("Coercing CEL header to v4...done\n");
  } else if (version == "3") {
    header$version <- "4";
  } else if (version == "4") {
  }

  # Check for supported versions
  if (header$version !=  "4") {
    stop("Failed create binary (XDA) CEL v4 file. Header object has a different version: ", header$version);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate/assign CEL header field 'total'
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  ncells <- header$cols*header$rows;
  if (!is.null(header$total)) {
    stopifnot(header$total == ncells);
  } else {
    header$total <- ncells;
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Check CEL header against CDF?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (isTRUE(cdf)) {
    # This might take a long time
    cdf <- findCdf(header$chiptype);
    if (is.null(cdf)) {
      warning("Skipping validation of CEL header against CDF header. Could not find a CDF file for this chip type: ", header$chiptype);
    }
  }
  if (is.character(cdf)) {
    cdfHeader <- readCdfHeader(cdf);
    cdfTotal <- cdfHeader$ncols * cdfHeader$nrows;
    if (ncells != cdfTotal) {
      warning("The number of cells in the CEL file does not match that of the CDF file: ", ncells, " != ", cdfTotal);
    }
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Open the file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  con <- file(filename, open="w+b");
  on.exit(close(con));


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Write CEL header
  #
  # The function takes care of redundant fields, unwrapping & wrapping...
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  writeCelHeader(con=con, header, verbose=verbose);
  
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # "Number of sub-grids."
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  writeInteger(con=con, nsubgrids);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Cell entries: (float, float, short) = 4+4+2=10 bytes/cell
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (ncells > 0) {
    seek(con=con, origin="current", where=10*ncells-1);
    writeBin(con=con, as.integer(0), size=1, endian="little");
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # "Masked entries - this consists of the XY coordinates of 
  #  those cells masked by the user. (short, short)" = 4 bytes/cells
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (header$nmasked > 0) {
    seek(con=con, origin="current", where=4*header$nmasked-1);
    writeBin(con=con, as.integer(0), size=1, endian="little");
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # "Outlier entries - this consists of the XY coordinates of those
  #  cells called outliers by the software. (short, short)" = 4 bytes/cells
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (header$noutliers > 0) {
    seek(con=con, origin="current", where=4*header$noutliers-1);
    writeBin(con=con, as.integer(0), size=1, endian="little");
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Sub-grid entries: (integer, integer, float, float, float, float, 
  # float, float, float, float, integer, integer, integer, integer)
  # = 6*integer + 8*float = 6*4+8*4 = 14*4 = 64 bytes/subgrid
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (nsubgrids > 0) {
    seek(con=con, origin="current", where=64*nsubgrids-1);
    writeBin(con=con, as.integer(0), size=1, endian="little");
  }

  invisible(filename);
} # createCel()


############################################################################
# HISTORY:
# 2012-09-26
# o Added argument 'cdf=NULL' to createCel(). Note, the previous
#   implementation corresponded to cdf=TRUE.
# o ROBUSTNESS: Now createCel() validates/sets CEL header field 'total'
#   based on 'cols' and 'rows'.
# 2007-08-16
# o Updated createCel() so it coerces the CEL header to version 4.
# 2006-09-07
# o Added Rdoc comments.
# o Added a small check against the CDF file, if it exists.
# o Managed to create the first CEL v4 file.  The tricky part is to create
#   a valid CEL v4 header, because there are quite a bit of redundant 
#   fields in there, which now are regenerated from the other fields to
#   make sure they are consistent.
# 2006-09-03
# o Created.
############################################################################

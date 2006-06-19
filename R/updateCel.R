#########################################################################/**
# @RdocFunction updateCel
#
# @title "Updates a CEL file"
#
# @synopsis 
# 
# \description{
#   @get "title".
# }
# 
# \arguments{
#   \item{filename}{The filename of the CEL file.}
#   \item{indices}{A @numeric @vector of cell (probe) indices specifying 
#     which cells to updated.}
#   \item{intensities}{A @numeric @vector of intensity values to be stored.
#     Alternatively, it can also be a named @data.frame or @matrix (or @list)
#     where the named columns (elements) are the fields to be updated.}
#   \item{stdvs}{A optional @numeric @vector.}
#   \item{pixels}{A optional @numeric @vector.}
#   \item{...}{Not used.}
#   \item{verbose}{An @integer specifying how much verbose details are
#     outputted.}
# }
# 
# \value{
#   Returns (invisibly) the pathname of the file updated.
# }
#
# \details{
#   Currently only binary (v4) CEL files are supported.
#   The current version of the method does not make use of the Fusion SDK,
#   but its own code to navigate and update the CEL file.
# }
#
# @examples "../incl/updateCel.Rex"
#
# @author
# 
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
updateCel <- function(filename, indices=NULL, intensities=NULL, stdvs=NULL, pixels=NULL, ..., verbose=FALSE) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Argument 'filename':
  if (!file.exists(filename)) {
    stop("Cannot update CEL file. File not found: ", filename);
  }

  header <- readCelHeader(filename);
  version <- header$version;
  if (!version %in% 4) {
    stop("Updating CEL v", version, " files is not supported: ", filename);
  }

  nbrOfCells <- header$total;

  # Argument 'indices':
  if (is.null(indices)) {
    nbrOfIndices <- nbrOfCells;
  } else {
    # A CEL file has one-based indices
    if (any(indices < 1 | indices > nbrOfCells))
      stop("Argument indices is out of range [1,", nbrOfCells, "].");
    nbrOfIndices <- length(indices);
  }

  # Argument 'intensities':
  if (is.matrix(intensities)) {
    intensities <- as.data.frame(intensities);
  }

  if (is.list(intensities) || is.data.frame(intensities)) {
    if (is.list(intensities)) {
      fields <- names(intensities);
    } else {
      fields <- colnames(intensities);
    }

    if (is.null(stdvs) && ("stdvs" %in% fields)) {
      stdvs <- intensities[["stdvs"]];
    }

    if (is.null(pixels) && ("pixels" %in% fields)) {
      pixels <- intensities[["pixels"]];
    }

    if ("intensities" %in% fields) {
      intensities <- intensities[["intensities"]];
    }
  }

  # Argument 'intensities':
  if (!is.null(intensities)) {
    intensities <- as.double(intensities);
    if (length(intensities) != nbrOfIndices) {
      stop("Number of 'intensities' values does not match the number of cell indices: ", length(intensities), " != ", nbrOfIndices);
    }
  }

  # Argument 'stdvs':
  if (!is.null(stdvs)) {
    stdvs <- as.double(stdvs);
    if (length(stdvs) != nbrOfIndices) {
      stop("Number of 'stdvs' values does not match the number of cell indices: ", length(stdvs), " != ", nbrOfIndices);
    }
  }

  # Argument 'pixels':
  if (!is.null(pixels)) {
    pixels <- as.integer(pixels);
    if (length(pixels) != nbrOfIndices) {
      stop("Number of 'pixels' values does not match the number of cell indices: ", length(pixels), " != ", nbrOfIndices);
    }
  }

  # Argument 'verbose':
  if (length(verbose) != 1)
    stop("Argument 'units' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'units' must be an integer: ", verbose);

  # Nothing to do?
  if (nbrOfIndices == 0) {
    return(invisible(filename));
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Reorder data such that it is written in optimal order
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  o <- order(indices);
  indices <- indices[o];
  if (!is.null(intensities))
    intensities <- intensities[o];
  if (!is.null(stdvs))
    stdvs <- stdvs[o];
  if (!is.null(pixels))
    pixels <- pixels[o];


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Write data to file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  version <- header$version;
  if (version == 4) {
    writeFloat <- function(con, value, ...) {
      writeBin(con=con, as.double(value), size=4, endian="little");
    }
  
    writeShort <- function(con, value, ...) {
      writeBin(con=con, as.integer(value), size=2, endian="little");
    }
  
    # Open CEL file
    con <- file(filename, open="r+b");
    on.exit(close(con));

    # Skip CEL header
    .readCelHeaderV4(con);

    # "Cell entries - this consists of an intensity value, standard 
    #  deviation value and pixel count for each cell in the array.
    #  The values are stored by row then column starting with the X=0, 
    #  Y=0 cell. As an example, the first five entries are for cells 
    #  defined by XY coordinates: (0,0), (1,0), (2,0), (3,0), (4,0).
    #  Type: (float, float, short) = 4 + 4 + 2 = 10 bytes / cell
    #  cellData <- c(readFloat(con), readFloat(con), readShort(con));
    # Current file position
    dataOffset <- seek(con);

    # File position offset for all cells to be updated
    sizeOfCell <- 10;
    offsets <- (dataOffset - sizeOfCell) + sizeOfCell*indices;

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Update 'intensities'
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (!is.null(intensities)) {
      for (kk in 1:nbrOfIndices) {
        seek(con, where=offsets[kk], rw="write");
        writeFloat(con, intensities[kk]);
      }
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Update 'stdvs'
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    offsets <- offsets + 4;
    if (!is.null(stdvs)) {
      for (kk in 1:nbrOfIndices) {
        seek(con, where=offsets[kk], rw="write");
        writeFloat(con, stdvs[kk]);
      }
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Update 'pixels'
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    offsets <- offsets + 4;
    if (!is.null(pixels)) {
      for (kk in 1:nbrOfIndices) {
        seek(con, where=offsets[kk], rw="write");
        writeShort(con, pixels[kk]);
      }
    }
  } # if (version ...)

  invisible(filename);
}


############################################################################
# HISTORY:
# 2006-06-19
# o Replace 'data' argument with arguments 'intensities', 'stdvs', and
#   'pixels'. /HB
# 2006-06-18
# o First version can update CEL v4 (binary) cell entries.  Note that this 
#   code does not make use of the Fusion SDK library.  This may updated in 
#   the future, but for now we just want something that works.
# o Created. /HB
############################################################################  

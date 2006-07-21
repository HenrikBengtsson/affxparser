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
#     which cells to updated.  If @NULL, all indices are considered.}
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
  if (is.null(indices)) {
    indices <- 1:nbrOfIndices;
  } else {
    o <- order(indices);
    indices <- indices[o];
    if (!is.null(intensities))
      intensities <- intensities[o];
    if (!is.null(stdvs))
      stdvs <- stdvs[o];
    if (!is.null(pixels))
      pixels <- pixels[o];
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Write data to file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  version <- header$version;
  if (version == 4) {
    # Open CEL file
    con <- file(filename, open="r+b");
    on.exit(close(con));

    # Skip CEL header
    hdr <- .readCelHeaderV4(con);

    # "Cell entries - this consists of an intensity value, standard 
    #  deviation value and pixel count for each cell in the array.
    #  The values are stored by row then column starting with the X=0, 
    #  Y=0 cell. As an example, the first five entries are for cells 
    #  defined by XY coordinates: (0,0), (1,0), (2,0), (3,0), (4,0).
    #  Type: (float, float, short) = 4 + 4 + 2 = 10 bytes / cell
    #  cellData <- c(readFloat(con), readFloat(con), readShort(con));
    sizeOfCell <- 10;

    # Current file position
    dataOffset <- seek(con);

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Read the data section of the CEL file
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # There is no need to read data before the first index
    minIndex <- min(indices);
    dataOffset <- dataOffset + sizeOfCell*(minIndex-1);
    indices <- indices - minIndex + 1;

    # There is no need to read beyond the last index
    maxIndex <- max(indices);
    seek(con, where=dataOffset);
    rawAll <- readBin(con=con, what="raw", n=sizeOfCell*maxIndex);
#print(matrix(rawAll[1:30], ncol=10, byrow=TRUE));

    tmp <- sizeOfCell*(indices-1);
    indices4 <- rep(tmp, each=4);
    indices2 <- rep(tmp, each=2);

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Update 'intensities'
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (!is.null(intensities)) {
      # Write floats (size=4) to raw vector
      raw <- raw(length=4*nbrOfIndices);
      raw <- writeBin(con=raw, intensities, size=4, endian="little");
      # Updated 'rawAll' accordingly
      idx <- indices4 + 1:4;
      rawAll[idx] <- raw;
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Update 'stdvs'
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (!is.null(stdvs)) {
      # Write floats (size=4) to raw vector
      raw <- raw(length=4*nbrOfIndices);
      raw <- writeBin(con=raw, stdvs, size=4, endian="little");

      # Updated 'rawAll' accordingly
      idx <- indices4 + 5:8;
      rawAll[idx] <- raw;
    }

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Update 'pixels'
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (!is.null(pixels)) {
      # Write short integers (size=2) to raw vector
      raw <- raw(length=2*nbrOfIndices);
      raw <- writeBin(con=raw, pixels, size=2, endian="little");

      # Updated 'rawAll' accordingly
      idx <- indices2 + 9:10;
      rawAll[idx] <- raw;
    }

#print(matrix(rawAll[1:30], ncol=10, byrow=TRUE));

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Write raw data back to file
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    seek(con, where=dataOffset, rw="write");
    writeBin(con=con, rawAll);
  } # if (version ...)

  invisible(filename);
}


############################################################################
# HISTORY:
# 2006-07-21
# o updateCel() was really slow when updating a large number of cells.
#   Now the idea is to write to raw vectors stored in memory.  By reading
#   the chunk of the CEL data section that is going to be updated as a raw
#   data vector and then updating this in memory first, and the re-write
#   that chuck of raw data to file, things are much faster.
# o BUG FIX: updateCel(..., indices=NULL) would generate an error, because
#   we tried to reorder by order(indices).
# 2006-06-19
# o Replace 'data' argument with arguments 'intensities', 'stdvs', and
#   'pixels'. /HB
# 2006-06-18
# o First version can update CEL v4 (binary) cell entries.  Note that this 
#   code does not make use of the Fusion SDK library.  This may updated in 
#   the future, but for now we just want something that works.
# o Created. /HB
############################################################################  

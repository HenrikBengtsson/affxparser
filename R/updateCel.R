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
#   \item{writeMap}{An optional write map.}
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
# @author "HB"
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
updateCel <- function(filename, indices=NULL, intensities=NULL, stdvs=NULL, pixels=NULL, writeMap=NULL, ..., verbose=0) {
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
    r <- range(indices);
    if (r[1] < 1 || r[2] > nbrOfCells) {
      stop("Argument 'indices' is out of range [1,", nbrOfCells, "]: ",
           "[", r[1], ",", r[2], "]");
    }
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
    if (!is.double(intensities))
      intensities <- as.double(intensities);
    if (length(intensities) != nbrOfIndices) {
      stop("Number of 'intensities' values does not match the number of cell indices: ", length(intensities), " != ", nbrOfIndices);
    }
  }

  # Argument 'stdvs':
  if (!is.null(stdvs)) {
    if (!is.double(stdvs))
      stdvs <- as.double(stdvs);
    if (length(stdvs) != nbrOfIndices) {
      stop("Number of 'stdvs' values does not match the number of cell indices: ", length(stdvs), " != ", nbrOfIndices);
    }
  }

  # Argument 'pixels':
  if (!is.null(pixels)) {
    if (!is.integer(pixels))
      pixels <- as.integer(pixels);
    if (length(pixels) != nbrOfIndices) {
      stop("Number of 'pixels' values does not match the number of cell indices: ", length(pixels), " != ", nbrOfIndices);
    }
  }

  # Argument 'writeMap':
  if (!is.null(writeMap)) {
    writeMap <- .assertMap(writeMap, nbrOfCells);
  }

  # Argument 'verbose':
  if (length(verbose) != 1)
    stop("Argument 'verbose' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'verbose' must be an integer: ", verbose);

  # Nothing to do?
  if (nbrOfIndices == 0) {
    return(invisible(filename));
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Reorder data such that it is written in optimal order
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  reorder <- TRUE;
  if (is.null(indices)) {
    # Has write map?
    if (is.null(writeMap)) {
      indices <- 1:nbrOfIndices;
      reorder <- FALSE;
    } else {
      indices <- writeMap;
    }
  }

  if (reorder) {
    if (verbose >= 2)
      cat("Re-ordering data for optimal write order...");
    o <- order(indices);
    indices <- indices[o];
    if (!is.null(intensities))
      intensities <- intensities[o];
    if (!is.null(stdvs))
      stdvs <- stdvs[o];
    if (!is.null(pixels))
      pixels <- pixels[o];
    o <- NULL; # Not needed anymore
    if (verbose >= 2)
      cat("done.\n");
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
    if (verbose >= 2)
      cat("Skipping to beginging of data section...");
    h <- .readCelHeaderV4(con);

    # "Cell entries - this consists of an intensity value, standard
    #  deviation value and pixel count for each cell in the array.
    #  The values are stored by row then column starting with the X=0,
    #  Y=0 cell. As an example, the first five entries are for cells
    #  defined by XY coordinates: (0,0), (1,0), (2,0), (3,0), (4,0).
    #  Type: (float, float, short) = 4 + 4 + 2 = 10 bytes / cell
    #  cellData <- c(readFloat(con), readFloat(con), readShort(con));
    sizeOfCell <- 10;

    # Current file position
    dataOffset <- seek(con, origin="start", rw="read");

    if (verbose >= 2)
      cat("done.\n");

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Update in chunks
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    CHUNK.SIZE <- 2^19; # = 524288 indices == 5.2Mb
    CHUNK.SIZE <- 2^20; # = 1048576 indices == 10.5Mb

    # Work with zero-based indices
    indices <- indices - 1;

    count <- 1;
    offset <- dataOffset;
    nbrOfChunks <- ceiling(length(indices) / CHUNK.SIZE);
    while (length(indices) > 0) {
      if (verbose >= 1) {
        cat(sprintf("Number of indices left: %d\n", length(indices)));
        cat(sprintf("Updating chunk #%d of %d...\n", count, nbrOfChunks));
      }

      # Recall: All indices are ordered!

      # Shift offset to the first index.
      firstIndex <- indices[1];
      offset <- offset + sizeOfCell*firstIndex;

      # Shift indices such that first index is zero.
      indices <- indices - firstIndex;

      # Get largest index
      maxIndex <- indices[length(indices)];
#      verbose && cat(verbose, "Largest index: ", maxIndex, "\n");

      # Identify the indices to update such no more than CHUNK.SIZE cells
      # are read/updated.
      n <- which.max(indices >= CHUNK.SIZE);
      if (n == 1)
        n <- length(indices);

      subset <- 1:n;

      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      # Read the data section of the CEL file
      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (verbose >= 1)
        cat("Reading chunk data section...");

      seek(con, origin="start", where=offset, rw="read");
      rawAll <- readBin(con=con, what="raw", n=sizeOfCell*(indices[n]+1));
      if (verbose >= 1)
        cat("done.\n");
      # Common to all fields
      raw <- NULL;

      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      # Update 'intensities'
      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (!is.null(intensities)) {
        if (verbose >= 1)
          cat("Updating 'intensities'...");
        # Write floats (size=4) to raw vector
        raw <- raw(length=4*n);
        raw <- writeBin(con=raw, intensities[subset], size=4, endian="little");
        intensities <- intensities[-subset]; # Not needed anymore

        # Updated 'rawAll' accordingly
        idx <- rep(sizeOfCell*indices[subset], each=4) + 1:4;
        rawAll[idx] <- raw;
        idx <- NULL; # Not needed anymore
        if (verbose >= 1)
          cat("done.\n");
      }

      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      # Update 'stdvs'
      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (!is.null(stdvs)) {
        if (verbose >= 1)
          cat("Updating 'stdvs'...");
        # Write floats (size=4) to raw vector
        if (length(raw) != 4*n)
          raw <- raw(length=4*n);
        raw <- writeBin(con=raw, stdvs[subset], size=4, endian="little");
        stdvs <- stdvs[-subset]; # Not needed anymore

        # Updated 'rawAll' accordingly
        idx <- rep(sizeOfCell*indices[subset], each=4) + 5:8;
        rawAll[idx] <- raw;
        idx <- NULL; # Not needed anymore
        if (verbose >= 1)
          cat("done.\n");
      }

      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      # Update 'pixels'
      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (!is.null(pixels)) {
        raw <- NULL; # Not needed anymore
        if (verbose >= 1)
          cat("Updating 'pixels'...");
        # Write short integers (size=2) to raw vector
        raw <- raw(length=2*n);
        raw <- writeBin(con=raw, pixels[subset], size=2, endian="little");
        pixels <- pixels[-subset]; # Not needed anymore

        # Updated 'rawAll' accordingly
        idx <- rep(sizeOfCell*indices[subset], each=2) + 9:10;
        rawAll[idx] <- raw;
        idx <- NULL; # Not needed anymore
        if (verbose >= 1)
          cat("done.\n");
      }
      raw <- NULL; # Not needed anymore

      # Remove updated indices
      indices <- indices[-subset];
      subset <- NULL; # Not needed anymore

      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      # Write raw data back to file
      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (verbose >= 1)
        cat("Writing chunk to data section...");
      seek(con, origin="start", where=offset, rw="write");
      writeBin(con=con, rawAll);
      if (verbose >= 1)
        cat("done.\n");

      rawAll <- NULL; # Not needed anymore

      if (verbose >= 1)
        cat(sprintf("Updating chunk #%d of %d...done\n", count, nbrOfChunks));
      count <- count + 1;
    } # while (...)
  } # if (version ...)

  invisible(filename);
}


############################################################################
# HISTORY:
# 2007-01-04
# o Added argument 'writeMap'.
# 2006-08-19
# o BUG FIX: Wow wow wow.  This one was tricky to find.  If not specifying
#   the 'rw' argument in seek() it defaults to "", which is not "read" as
#   I naively though (because I did not read the inner details of ?seek),
#   but the latest call to seek.  In other words, since I at the end of
#   every "chunk" loop call seek(..., rw="write") the seek(..., [rw=""])
#   was equal to a seek(..., rw="write"), but I wanted seek(..., rw="read")!
#   That made updateCel() do funny things and write to the wrongs parts
#   of the file etc.
# 2006-08-18
# o BUG FIX: The new implementation where data was written to raw vectors
#   was incorrect.  A lot of extra zeros was written.  The "eastern egg" in
#   the updated example contains an image from http://tpo.berkeley.edu/.
# 2006-08-14
# o BUG FIX: updateCel() would in some cases give "Error: subscript out of
#   bounds" when writing the last chunk.
# 2006-07-22
# o Update updateCel() to update data in chunks, because updating the
#   complete data section is expensive.  For example, a 500K chip has
#   6553600 cells each of size 10 bytes, i.e. >65Mb or raw memory.  With
#   copying etc it costs >100-200Mb of memory to update a CEL file if only
#   the first *and* the last cell is updated.  Now it should only be of
#   the order of 10-20Mb per chunk.
# o Added verbose output to updateCel().
# o Now updateCel() deallocates objects as soon as possible in order to
#   free up as much memory as possible. Had memory problems with the 500K's.
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

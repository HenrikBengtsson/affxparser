#########################################################################/**
# @RdocFunction readCdfUnitsWriteMap
#
# @title "Generates an Affymetrix cell-index write map from a CDF file"
#
# @synopsis
#
# \description{
#   @get "title".
#
#   The purpose of this method is to provide a re-ordering of cell elements
#   such that cells in units (probesets) can be stored in contigous blocks.
#   When reading cell elements unit by unit, minimal file re-position is
#   required resulting in a faster reading.
#
#   Note: At the moment does this package not provide methods to
#   write/reorder CEL files.  In the meanwhile, you have to write
#   and re-read using your own file format.  That's not too hard using
#   \code{writeBin()} and @see "base::readBin".
# }
#
# \arguments{
#   \item{filename}{The pathname of the CDF file.}
#   \item{units}{An @integer @vector of unit indices specifying which units
#     to listed first.  All other units are added in order at the end.
#     If @NULL, units are in order.}
#   \item{...}{Additional arguments passed to @see "readCdfUnits".}
#   \item{verbose}{Either a @logical, a @numeric, or a @see "R.utils::Verbose"
#     object specifying how much verbose/debug information is written to
#     standard output. If a Verbose object, how detailed the information is
#     is specified by the threshold level of the object. If a numeric, the
#     value is used to set the threshold of a new Verbose object. If @TRUE,
#     the threshold is set to -1 (minimal). If @FALSE, no output is written
#     (and neither is the \pkg{R.utils} package required).}
# }
#
# \value{
#   A @integer @vector which is a \emph{write} map.
# }
#
# @author "HB"
#
# \examples{
#   @include "../incl/readCdfUnitsWriteMap.Rex"
#
#   @include "../incl/readCdfUnitsWriteMap.2.Rex"
# }
#
# \seealso{
#   To invert maps, see @see "invertMap".
#   @see "readCel" and @see "readCelUnits".
# }
#
# @keyword "file"
# @keyword "IO"
# @keyword "internal"
#*/#########################################################################
readCdfUnitsWriteMap <- function(filename, units=NULL, ..., verbose=FALSE) {
  # To please R CMD check
  Arguments <- enter <- exit <- NULL;
  rm(list=c("Arguments", "enter", "exit"));

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'filename':
  # Replace '~':s
  filename <- file.path(dirname(filename), basename(filename));

  # Argument 'units':
  if (!is.null(units)) {
    units <- as.numeric(units);
    if (any(is.na(units))) {
      stop("Argument 'units' contains NAs");
    }
    nok <- (units < 1);
    if (any(nok)) {
      nok <- paste(units[nok], collapse=", ");
      stop("Argument 'units' contains non-positive indices: ", nok);
    }
    nok <- duplicated(units);
    if (any(nok)) {
      nok <- paste(units[nok], collapse=", ");
      stop("Argument 'units' contains duplicated indices: ", nok);
    }
  }

  # Argument 'verbose':
  if (!identical(verbose, FALSE)) {
    require(R.utils) || stop("Package not available: R.utils");
    verbose <- Arguments$getVerbose(verbose);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read CDF header and process 'units' further
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  header <- readCdfHeader(filename);
  nbrOfCells <- header$ncols * header$nrows;
  nbrOfUnits <- header$probesets;

  nok <- (units > nbrOfUnits);
  if (any(nok)) {
    nok <- paste(units[nok], collapse=", ");
    stop("Argument 'units' contains indices out of range [1,", nbrOfUnits,
                                                               "]: ", nok);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Read cell indices unit by unit
  verbose && enter(verbose, "Reading cell indices unit by unit from CDF file");
  indices <- readCdfCellIndices(filename, units=units, ..., verbose=FALSE);
  verbose && exit(verbose);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Return cell indices according to 'units'
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (!is.null(units)) {
    verbose && enter(verbose, "Reordering by units");
    # Was only a subset of units specified?
    if (length(units) != nbrOfUnits) {
      verbose && enter(verbose, "Adding missing unit indices");
      allUnits <- 1:nbrOfUnits;
      missing <- setdiff(allUnits, units);
      units <- c(units, missing);
      missing <- allUnits <- NULL; # Not needed anymore
      verbose && exit(verbose);
    }

    # Now, reorder the units (here 'indices') accordingly.
    indices <- indices[units];

    units <- NULL; # Not needed anymore
    verbose && exit(verbose);
  }

  indices <- unlist(indices, use.names=FALSE);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Create index map
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  verbose && enter(verbose, "Adding missing cell indices");
  # Add non-probeset cells to the end.
  # (Note that readCdfCellIndices() do not read these guys.)
  allIndices <- 1:nbrOfCells;
  missing <- setdiff(allIndices, indices);
  indices <- c(indices, missing);
  missing <- NULL; # Not needed anymore
  verbose && exit(verbose);

  # Returns the write map
  indices;
}


############################################################################
# HISTORY:
# 2006-09-07
# o BUG FIX: Tried to access fields 'cols' and 'rows' instead of 'ncols'
#   and 'nrows' in the CDF header.
# 2006-04-01
# o Now using readCdfCellIndices() to get cell indices.
# 2006-03-30
# o Redefined and renamed method to readCdfUnitsWriteMap().
# o Removed argument 'writeMap'.  This is possible because the new
#   invertMap() is so fast.
# 2006-03-28
# o Unit and cell indices are now one-based. /HB
# 2006-03-14
# o Updated code to make use of package R.utils only if it is available.
# o Added argument 'writeMap'.
# o Added more Rdoc comments and two examples with summaries etc.
# 2006-03-06
# o Added argument 'units' to read a subset of units or the units as, say,
#   they are order by chromsomal position.
# 2006-03-04
# o Removed all gc(). They slow down quite a bit.
# o Created.
############################################################################

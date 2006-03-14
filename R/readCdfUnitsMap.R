#########################################################################/**
# @RdocFunction readCdfUnitsMap
#
# @title "Generates an Affymetrix cell-index map from a CDF file"
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
#   and reread using your own file format.  That's not too hard using
#   \code{writeBin()} and @see "base::readBin".
# }
# 
# \arguments{
#   \item{pathname}{The pathname of the CDF file.}
#   \item{units}{An @integer @vector of (zero-based) unit indices 
#     specifying which units to listed first.  All other units are added
#     in order at the end.  If @NULL, units are in order.}
#   \item{...}{Additional arguments passed to @see "readCdfUnits".}
#   \item{writeMap}{If @TRUE, the write map, that is, the map for writing
#     cell elements is returned, otherwise the read map is returned.}
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
#   An @integer @vector of length \eqn{n} with unique elements in 
#   \eqn{[1,n]}, where \eqn{n} is the number of cells on the array.
# }
#
# \details{
#   If \code{writeMap} is @TRUE, the cell-index map for writing cell 
#   elements is returned.  If @FALSE, the map for reading cell elements is
#   returned.  The relationship between the read map and the write map is
#   \code{readMap <- order(writeMap)} and \code{writeMap <- order(readMap)}. 
#   Yes, \code{map <- order(order(map))}.
# }
#
# @author
# 
# \examples{
#   @include "../incl/readCdfUnitsMap.Rex"
#
#   @include "../incl/readCdfUnitsMap.2.Rex"
# }
# 
# \seealso{
#   @see "readCel" and @see "readCelUnits".
# }
# 
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCdfUnitsMap <- function(pathname, units=NULL, ..., writeMap=FALSE, verbose=FALSE) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Argument 'pathname':
  # Replace '~':s
  pathname <- file.path(dirname(pathname), basename(pathname));

  # Argument 'units':
  if (!is.null(units)) {
    units <- as.numeric(units);
    if (any(is.na(units))) {
      stop("Argument 'units' contains NAs");
    }
    nok <- (units < 0);
    if (any(nok)) {
      nok <- paste(units[nok], collapse=", ");
      stop("Argument 'units' contains negative indices: ", nok);
    }
    nok <- duplicated(units);
    if (any(nok)) {
      nok <- paste(units[nok], collapse=", ");
      stop("Argument 'units' contains duplicated indices: ", nok);
    }
  }

  # Argument 'writeMap':
  writeMap <- as.logical(writeMap);

  # Argument 'verbose':
  if (!identical(verbose, FALSE)) {
    require(R.utils) || stop("Package not available: R.utils");
    verbose <- Arguments$getVerbose(verbose);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read CDF header and process 'units' further
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  header <- readCdfHeader(pathname);
  nbrOfCells <- header$cols * header$rows;
  nbrOfUnits <- header$probesets;

  nok <- (units >= nbrOfUnits);
  if (any(nok)) {
    nok <- paste(units[nok], collapse=", ");
    stop("Argument 'units' contains indices out of range [0,", 
                                                 nbrOfUnits-1, "]: ", nok);
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read cell indices unit by unit
  verbose && enter(verbose, "Reading cell indices unit by unit from CDF file");
  cells <- readCdfUnits(pathname, readCells=TRUE, readXY=FALSE, 
                      readBases=FALSE, readExpos=FALSE, readType=FALSE, 
                                  readDirection=FALSE, ..., verbose=FALSE);
  verbose && exit(verbose);
  

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Return cell indices according to 'units'
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  if (!is.null(units)) {
    verbose && enter(verbose, "Reordering by units");
    # Was only a subset of units specified?
    if (length(units) != nbrOfUnits) {
      verbose && enter(verbose, "Adding missing unit indices");
      allUnits <- seq(from=0, to=nbrOfUnits-1);
      missing <- setdiff(allUnits, units);
      units <- c(units, missing);
      rm(missing, allUnits);
      verbose && exit(verbose);
    }

    # Now, reorder the 'cells' accordingly.
    cells <- cells[units];

    rm(units);
    verbose && exit(verbose);
  }

  cells <- unlist(cells, use.names=FALSE); 

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Create index map
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Note: Although the cell indices are zero-based, the remapping function
  # is not, i.e. 'cells' is in [0,n-1] and 'map' is in [1,n].

  verbose && enter(verbose, "Adding missing cell indices");
  # Add non-probeset cells to the end.  
  # (Note that readCdfUnits() do not read these guys.)
  allCells <- 0:(nbrOfCells-1);  # Note, zero-based
  missing <- setdiff(allCells, cells);
  cells <- c(cells, missing);
  rm(missing);
  verbose && exit(verbose);

  if (writeMap) {
    # 'cells' in [0,n-1], but 'map' is in [1,n]!
    cells <- cells + as.integer(1);
  } else {
    verbose && enter(verbose, "Generating index map");
    # Note, order() can be used, but match() is 7-8 times faster!
    cells <- match(allCells, cells);  # This is the slow part.
    verbose && exit(verbose);
  }

  cells;    
}


############################################################################
# HISTORY:
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

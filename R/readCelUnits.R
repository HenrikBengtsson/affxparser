#########################################################################/**
# @RdocFunction readCelUnits
#
# @title "Reads probe-level data ordered as units (probesets) from one or several Affymetrix CEL files"
#
# @synopsis 
# 
# \description{
#   @get "title" by using the unit and group definitions in the 
#   corresponding Affymetrix CDF file.
# }
# 
# \arguments{
#   \item{filenames}{The filenames of the CEL files.}
#   \item{units}{An @integer @vector of (zero-based) unit indices 
#     specifying which units to be read.  If @NULL, all units are read.}
#   \item{readOutliers, readMasked, ...}{Arguments passed to 
#     low-level method @see "affxparser::readCel", e.g. \code{readXY}
#     and \code{readStdvs}.}
#   \item{transforms}{A @list of exactly \code{length(filenames)}
#     @functions.  If @NULL, no transformation is performed.
#     Intensities read are passed through the corresponding transform
#     function before being returned.}
#   \item{cdf}{A @character filename of a CDF file, or a CDF @list
#     structure.  If @NULL, the CDF file is searched for by
#     @see "findCdf" first starting from the current directory and
#     then from the directory where the first CEL file is.}
#   \item{stratifyBy}{Argument passed to low-level method 
#     @see "affxparser::readCdfUnits".}
#   \item{addDimnames}{If @TRUE, dimension names are added to arrays,
#     otherwise not.  The size of the returned CEL structure in bytes 
#     increases by 30-40\% with dimension names.}
#   \item{map}{A @vector remapping cell indices to file indices.  
#     If @NULL, no mapping is used.}
#   \item{drop}{If @TRUE and only one array is read, the elements of
#     the group field do \emph{not} have an array dimension.}
#   \item{verbose}{Either a @logical, a @numeric, or a @see "R.utils::Verbose"
#     object specifying how much verbose/debug information is written to
#     standard output. If a Verbose object, how detailed the information is
#     is specified by the threshold level of the object. If a numeric, the
#     value is used to set the threshold of a new Verbose object. If @TRUE, 
#     the threshold is set to -1 (minimal). If @FALSE, no output is written
#     (and neither is the \pkg{R.utils} package required).
#   }
# }
# 
# \value{
#   A named @list where the names corresponds to the names of the units
#   read.  Each element of the @list is in turn a @list structure 
#   with groups (aka blocks).
# }
#
# @author
# 
# @examples "../incl/readCelUnits.Rex"
# 
# \seealso{
#   The @see "readCel" method is used internally.
# }
# 
# \references{
#   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
#       June 14, 2005.
#       \url{http://www.affymetrix.com/support/developer/}
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCelUnits <- function(filenames, units=NULL, readOutliers=FALSE, readMasked=FALSE, ..., transforms=NULL, cdf=NULL, stratifyBy=c("nothing", "pmmm", "pm", "mm"), addDimnames=FALSE, map=NULL, drop=TRUE, verbose=FALSE) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Argument 'filenames':
  filenames <- file.path(dirname(filenames), basename(filenames));
  missing <- filenames[!file.exists(filenames)];
  if (length(missing)) {
    stop("File(s) not found: ", paste(missing, collapse=", "));
  }

  # Argument 'units' and 'cdf':
  if (is.list(cdf) && !is.null(units)) {
    stop("Arguments 'units' must not be specified if argument 'cdf' is a CDF list structure.");
  }

  # Argument 'units':
  if (is.null(units)) {
  } else if (is.numeric(units)) {
    units <- as.integer(units);
    if (any(units < 0))
      stop("Argument 'units' contains negative indices.");
  } else {
    stop("Argument 'units' must be numeric or NULL: ", class(units)[1]);
  }

  # Argument 'cdf':
  searchForCdf <- FALSE;
  if (is.null(cdf)) {
    searchForCdf <- TRUE;
  } else if (is.character(cdf)) {
    cdfFile <- file.path(dirname(cdf), basename(cdf));
    if (!file.exists(cdfFile))
      stop("File not found: ", cdfFile);
    cdf <- NULL;
  } else if (is.list(cdf)) {
    aUnit <- cdf[[1]];
    if (!is.list(aUnit))
      stop("Argument 'cdf' is of unknown format: First unit is not a list.");

    groups <- aUnit$groups;
    if (!is.list(groups))
      stop("Argument 'cdf' is of unknown format: Units Does not contain the list 'groups'.");

    # Check for group fields 'cells' or 'x' & 'y' in one of the groups.
    aGroup <- groups[[1]];

    fields <- names(aGroup);
    if ("cells" %in% fields) {
      cdfType <- "cells";
    } else if (all(c("x", "y") %in% fields)) {
      cdfType <- "x";
      searchForCdf <- TRUE;
    } else {
      stop("Argument 'cdf' is of unknown format: The groups contains neither the fields 'cells' nor ('x' and 'y').");
    }
    rm(aUnit, groups, aGroup);
  } else {
    stop("Argument 'cdf' must be a filename, a CDF list structure or NULL: ", mode(cdf));
  }

  # Argument 'map':
  if (!is.null(map)) {
    # Cannot check map indices without knowing the array.  Is it worth 
    # reading such details already here?
  }

  # Argument 'drop':
  drop <- as.logical(drop);

  # Argument 'addDimnames':
  addDimnames <- as.logical(addDimnames);

  nbrOfArrays <- length(filenames);

  # Argument 'transforms':
  if (is.null(transforms)) {
    hasTransforms <- FALSE;
  } else if (is.list(transforms)) {
    if (length(transforms) != nbrOfArrays) {
      stop("Length of argument 'transforms' does not match the number of arrays: ", 
                                   length(transforms), " != ", nbrOfArrays);
    }
    for (transform in transforms) {
      if (!is.function(transform))
        stop("Argument 'transforms' must be a list of functions.");
    }
    hasTransforms <- TRUE;
  } else {
    stop("Argument 'transforms' must be a list of functions or NULL.");
  }

  # Argument 'stratifyBy':
  stratifyBy <- match.arg(stratifyBy);


  # Argument 'verbose': (Utilized the Verbose class in R.utils if available)
  if (!identical(verbose, FALSE)) {
    require(R.utils) || stop("Package not available: R.utils");
    verbose <- Arguments$getVerbose(verbose);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # 0. Search for CDF file?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (searchForCdf) {
    verbose && enter(verbose, "Searching for CDF file");

    verbose && enter(verbose, "Reading chip type from first CEL file");
    celHeader <- readCelHeader(filenames[1]);
    chipType <- celHeader$chiptype;
    verbose && exit(verbose);

    verbose && enter(verbose, "Searching for chip type '", chipType, "'");
    cdfFile <- findCdf(chipType=chipType);
    if (length(cdfFile) == 0) {
      # If not found, try also where the first CEL file is
      opwd <- getwd();
      on.exit(setwd(opwd));
      setwd(dirname(filenames[1]));
      cdfFile <- findCdf(chipType=chipType);
      setwd(opwd);
    }
    verbose && exit(verbose);
    if (length(cdfFile) == 0)
      stop("No CDF file for chip type found: ", chipType);

    verbose && exit(verbose);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # 1. Read cell indices for units of interest from the CDF file?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (is.null(cdf)) {
    verbose && enter(verbose, "Reading cell indices from CDF file");
    cdf <- readCdfUnits(cdfFile, units=units, readXY=FALSE, readCells=TRUE, 
                       readBases=FALSE, readDirection=FALSE, readExpos=FALSE,
                                      readType=FALSE, stratifyBy=stratifyBy);
    verbose && exit(verbose);

    # Assume 'cdf' contains only "cells" fields.
    cells <- unlist(cdf, use.names=FALSE);
  } else {
    if (cdfType == "cells") {
      # Clean up CDF list structure from other fields than "cells".
      cdf <- lapply(cdf, FUN=function(unit) {
        lapply(unit, FUN=function(group) {
          list(cells=group$cells);
        })
      })
      cells <- unlist(cdf, use.names=FALSE);
    } else {
      verbose && enter(verbose, "Calculating cell indices from (x,y) positions");
      verbose && enter(verbose, "Reading chip layout from CDF file");
      cdfHeader <- readCdfHeader(cdfFile);
      ncol <- cdfHeader$cols;
      verbose && exit(verbose);
      x <- unlist(lapply(cdf, FUN=function(u) {
        unlist(lapply(u$groups, FUN=function(group) {
          group$x;
        }), use.names=FALSE)
      }), use.names=FALSE)
      y <- unlist(lapply(cdf, FUN=function(u) {
        unlist(lapply(u$groups, FUN=function(group) {
          group$y;
        }), use.names=FALSE)
      }), use.names=FALSE)
      cells <- as.integer(y * ncol + x);
      rm(x,y);
      verbose && exit(verbose);
    }
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # 2. Remapping cell indices?
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (!is.null(map)) {
    verbose && enter(verbose, "Remapping cell indices");
    cells <- map[cells+1]-1;
    verbose && exit(verbose);
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # 3. Read signals of the cells of interest from the CEL file(s)
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  nbrOfCells <- length(cells);
  nbrOfUnits <- length(cdf);

  verbose && enter(verbose, "Reading ", nbrOfUnits, "*", nbrOfCells/nbrOfUnits, "=", nbrOfCells, " cells from ", nbrOfArrays, " CEL files");

  matrixFields <- c("intensities", "stdvs", "pixels");
  integerFields <- "pixels";
  doubleFields <- setdiff(matrixFields, integerFields);

  for (kk in seq(length=nbrOfArrays)) {
    filename <- filenames[kk];

    verbose && enter(verbose, "Reading CEL data for array #", kk);
    celTmp <- readCel(filename, indices=cells, readOutliers=readOutliers, readMasked=readMasked, ...);
    verbose && exit(verbose);

    if (kk == 1) {
      verbose && enter(verbose, "Allocating return structure");
      # Allocate the return list structure
      celTmp$header <- NULL;
      cel <- vector("list", length(celTmp));
      names(cel) <- names(celTmp);

      # Update list of special fields
      matrixFields <- intersect(names(cel), matrixFields);
      doubleFields <- intersect(matrixFields, doubleFields);
      integerFields <- intersect(matrixFields, integerFields);
      listFields <- setdiff(names(cel), matrixFields); 

      if (nbrOfArrays >= 2 && !drop) {
        for (name in listFields)
          cel[[name]] <- vector("list", nbrOfArrays);
      }

      for (name in doubleFields)
        cel[[name]] <- matrix(as.double(0), nrow=nbrOfCells, ncol=nbrOfArrays);

      for (name in integerFields)
        cel[[name]] <- matrix(as.integer(0), nrow=nbrOfCells, ncol=nbrOfArrays);
      verbose && exit(verbose);
    } 

    for (name in matrixFields) {
      value <- celTmp[[name]];
      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      # Transform signals?
      # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (hasTransforms && name == "intensities") {
        verbose && enter(verbose, "Transform signals for array #", kk);
        value <- transforms[[kk]](value);
        verbose && exit(verbose);
      }
      cel[[name]][,kk] <- value;
    }

    for (name in setdiff(matrixFields, "intensities")) {
      cel[[name]][,kk] <- celTmp[[name]];
    }

    for (name in listFields) {
      if (!is.null(celTmp[[name]]))
        cel[[name]][[kk]] <- celTmp[[name]];
    }

    rm(celTmp);
  }
  verbose && exit(verbose);

  rm(cells);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # 3. Structure CEL data in units and groups according to the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  verbose && enter(verbose, "Structuring data by units and groups");

  fields <- vector("list", length(cel));
  names(fields) <- names(cel);

  addDim <- (nbrOfArrays >= 2 || !drop);

  seqOfArrays <- list(1:nbrOfArrays);

  offset <- 0;
  cel <- lapply(cdf, FUN=function(u) {
    lapply(u$groups, FUN=function(g) {
      # Same dimensions of all fields
      field <- g[[1]];
      ncells <- length(field);
      idxs <- offset + 1:ncells;
      offset <<- offset + ncells;

      # Get the target dimension
      dim <- dim(field);
      if (is.null(dim))
        dim <- ncells;
      if (addDimnames)
        dimnames <- dimnames(field);

      # Add an extra dimension for arrays?
      if (addDim) {
        if (addDimnames)
          dimnames <- c(dimnames, seqOfArrays);
        dim <- c(dim, nbrOfArrays);
      }

      # Update all fields with dimensions
      for (name in matrixFields) {
        values <- cel[[name]][idxs,];                 # cel[[...]] is slow!
        dim(values) <- dim;
        if (addDimnames)
          dimnames(values) <- dimnames;
        fields[[name]] <- values;
      }

      # Update all dimension-less fields
      for (name in listFields) {
        values <- cel[[name]];                        # cel[[...]] is slow!
        if (!is.null(values))
          fields[[name]] <- values;
      }

      fields;
    });
  });

  verbose && exit(verbose);

  cel;
}


############################################################################
# HISTORY:
# 2006-03-14
# o Updated code to make use of package R.utils only if it is available.
# 2006-03-08
# o Removed the usage of Arguments of R.utils.  This is because we might
#   move this function to the affxparser package.  Still to be removed is
#   the use of the Verbose class.
# 2006-03-04
# o Added argument 'map'.
# o Removed all gc(). They slow down quite a bit.
# 2006-02-27 [HB]
# o BUG FIX: It was only stratifyBy="pmmm" that worked if more than one
#   array was read.
# 2006-02-23 [HB]
# o The restructuring code is now more generic, e.g. it does not require
#   the 'stratifyBy' argument and can append multiple arrays of any 
#   dimensions.
# o Now the CDF file is search for where the CEL files lives too.
# 2006-02-22 [HB]
# o First test where argument 'cdf' is a CDF structure.  Fixed some bugs,
#   but it works now.
# o Simple redundancy test: The new code and the updated affxparser package
#   works with the new aroma.affymetrix/rsp/ GUI.
# o Now argument 'cdf' is checked to contain either 'cells' or 'x' & 'y'
#   group fields.  If 'x' and 'y', the cell indices are calculated from 
#   (x,y) and the chip layout obtained from the header of CDF file, which 
#   has been searched for.
# 2006-02-21 [HB]
# o TO DO: Re implement all of this in a C function to speed things up
#   further; it is better to put values in the right position from the
#   beginning.
# o Added arguments 'transforms' to be able to transform all probe signals
#   at once.  This improves the speed further.
# o Removed annotation of PM/MM dimension when 'stratifyBy="pmmm", because
#   the resulting object increases ~33% in size.
# o Improved speed for restructuring cell data about 20-25%.
# o Now it is possible to read multiple CEL files.
# o Making use of new 'readCells' in readCdfUnits(), which is much faster.
# o Replaced argument 'splitPmMm' with 'stratifyBy'.  This speeds up if 
#   reading PM (or MM) only.
# o BUG FIX: 'splitPmMm=TRUE' allocated PMs and MMs incorrectly.  The reason
#   was that the indices are already in the correct PM/MM order from the
#   splitPmMm in readCdfUnits() from which the (x,y) to cell indices are
#   calculated.
# o Updated to make use of the latest affxparser.
# 2006-01-24 [HB]
# o BUG FIX: Made some modifications a few days ago that introduced missing
#   variables etc.
# 2006-01-21 [HB]
# o Added Rdoc comments.
# 2006-01-16 [HB]
# o Created by HB.
############################################################################  

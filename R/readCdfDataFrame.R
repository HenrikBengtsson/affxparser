#########################################################################/**
# @RdocFunction readCdfDataFrame
#
# @title "Reads units (probesets) from an Affymetrix CDF file"
#
# @synopsis 
# 
# \description{
#  @get "title". Gets all or a subset of units (probesets).
# }
# 
# \arguments{
#  \item{filename}{The filename of the CDF file.}
#  \item{units}{An @integer @vector of unit indices
#    specifying which units to be read.  If @NULL, all units are read.}
#  \item{verbose}{An @integer specifying the verbose level. If 0, the
#    file is parsed quietly.  The higher numbers, the more details.}
# }
# 
# \value{
#   An NxK @data.frame.
# }
#
# \author{
# }
# 
# @examples "../incl/readCdfDataFrame.Rex"
# 
# \seealso{
#   For retrieving the CDF as a @list structure, see @see "readCdfUnits".
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
readCdfDataFrame <- function(filename, units=NULL, groups=NULL, cells=NULL, fields=NULL, drop=TRUE, verbose=0) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Argument 'filename':
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename))
    stop("File not found: ", filename);

  # Argument 'units':
  if (is.null(units)) {
  } else if (is.numeric(units)) {
    units <- as.integer(units);
    if (any(units < 1))
      stop("Argument 'units' contains non-positive indices.");
  } else {
    stop("Argument 'units' must be numeric or NULL: ", class(units)[1]);
  }

  # Argument 'groups':
  if (is.null(groups)) {
  } else if (is.numeric(groups)) {
    groups <- as.integer(groups);
    if (any(groups < 1))
      stop("Argument 'groups' contains non-positive indices.");
  } else {
    stop("Argument 'groups' must be numeric or NULL: ", class(groups)[1]);
  }

  # Argument 'fields':
##   knownUnitFields <- c("unit", "unitName", "unitDirection", "nbrOfUnitAtoms", "unitSize", "unitNumber", "unitType", "nbrOfGroups", "mutationType");
##   knownGroupFields <- c("group", "groupName", "nbrOfGroupAtoms", "groupSize", "firstAtom", "lastAtom", "groupDirection");
##   knownCellFields <- c("cell", "x", "y", "probeSequence", "feat", "qual", "expos", "pos", "cbase", "pbase", "tbase", "atom", "index");

  if (is.null(fields)) {
    knownUnitFields <- c("unit", "unitName", "unitType", 
                         "unitDirection", "unitAtomNumbers");
    knownGroupFields <- c("group", "groupName", "groupDirection",
                          "groupAtomNumbers");
    knownCellFields <- c("cell", "x", "y", "pbase", "tbase", 
                         "indexPos", "atom");
    fields <- c(knownUnitFields, knownGroupFields, knownCellFields);
  }
  
  # Argument 'verbose':
  if (length(verbose) != 1)
    stop("Argument 'verbose' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'verbose' must be an integer: ", verbose);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Prepare the arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  readFields <- c(fields, "cell");  # Need to read one cell field!
  readFields <- unique(readFields);
  
  # Unit fields
  readUnitType <- ("unitType" %in% readFields);
  readUnitDirection <- ("unitDirection" %in% readFields);
  readUnitNumber <- ("unitNumber" %in% readFields);
  readUnitAtomNumbers <- ("unitAtomNumbers" %in% readFields);

  # Group fields
  readGroupAtomNumbers <- ("groupAtomNumbers" %in% readFields);
  readGroupDirection <- ("groupDirection" %in% readFields);

  # Cell fields
  readXY <- any(c("x", "y") %in% readFields);
  readIndices <- ("cell" %in% readFields);
  readBases <- any(c("tbase", "pbase") %in% readFields);
  readIndexpos <- ("indexPos" %in% readFields);
  readAtoms <- ("atom" %in% readFields);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Query the CDF
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  cdf <- readCdf(filename, units=units, readXY=readXY, readBases=readBases, readIndexpos=readIndexpos, readAtoms=readAtoms, readUnitType=readUnitType, readUnitDirection=readUnitDirection, readUnitNumber=readUnitNumber, readUnitAtomNumbers=readUnitAtomNumbers, readGroupAtomNumbers=readGroupAtomNumbers, readGroupDirection=readGroupDirection, readIndices=readIndices, verbose=verbose);

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Flatten CDF list structure unit by unit
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  if (is.null(units))
    units <- seq(along=cdf);
  groupIdxs <- groups;
  for (uu in seq(along=cdf)) {
    unit <- cdf[[uu]];
    unitName <- names(cdf)[uu];

    groups <- unit[["groups"]];
    unit[["groups"]] <- NULL;

    # Translate unit names
    names <- names(unit);
    names <- sub("unitdirection", "unitDirection", names); 
    names <- sub("unittype", "unitType", names); 
    names <- sub("ncellsperatom", "unitNbrOfCellsPerAtom", names); 
    names <- sub("ncells", "unitNbrOfCells", names); 
    names <- sub("natoms", "unitNbrOfAtoms", names); 
    names <- sub("unitnumber", "unitNumber", names); 
    names(unit) <- names;

    unitData <- list(unit=units[uu], unitName=unitName);
    unitData <- c(unitData, unit);

    # Extract groups of interest?
    if (is.null(groupIdxs)) {
      ggs <- seq(along=groups);
    } else {
      keep <- which(seq(along=groups) %in% groupIdxs);
      groups <- groups[keep];
      ggs <- groupIdxs;
    }

    # Flatten (group, cell) data
    groupNames <- names(groups);
    for (gg in seq(along=ggs)) {
      group <- groups[[gg]];
      groupName <- groupNames[gg];

      groupData <- list(group=ggs[gg], groupName=groupName);

      # Extract group fields
      keys <- c("groupdirection", "natoms", "ncellsperatom", "ncells");
      idxs <- which(names(group) %in% keys);
      if (length(idxs) > 0) {
        groupData <- c(groupData, group[idxs]);
        group <- group[-idxs];
      }

      # Translate unit names
      names <- names(groupData);
      names <- sub("groupdirection", "groupDirection", names); 
      names <- sub("natoms", "groupNbrOfAtoms", names); 
      names <- sub("ncellsperatom", "groupNbrOfCellsPerAtom", names); 
      names <- sub("ncells", "groupNbrOfCells", names); 
      names(groupData) <- names;


      # Extract cell fields
      cellData <- as.data.frame(group, stringsAsFactors=FALSE);

      # Extract cells of interest?
      if (!is.null(cells)) {
        cellData <- cellData[seq(length=nrow(cellData)) %in% cells,,drop=FALSE];
      }

      # Expand group fields
      nbrOfCells <- nrow(cellData);
      for (key in names(groupData)) {
        groupData[[key]] <- rep(groupData[[key]], times=nbrOfCells);
      }
      groupData <- as.data.frame(groupData, stringsAsFactors=FALSE);

      group <- cbind(groupData, cellData);
      rm(groupData, cellData);

      groups[[gg]] <- group;
    }

    # Stack (rbind) groups
    stackedGroups <- groups[[1]];
    groups <- groups[-1];
    while (length(groups) > 0) {
      stackedGroups <- rbind(stackedGroups, groups[[1]]);
      groups <- groups[-1];
    }
    rm(groups);

    nbrOfCells <- nrow(stackedGroups);
    for (key in names(unitData)) {
      unitData[[key]] <- rep(unitData[[key]], times=nbrOfCells);
    }
    unitData <- as.data.frame(unitData, stringsAsFactors=FALSE);

    unit <- cbind(unitData, stackedGroups);
    rm(unitData, stackedGroups);

    cdf[[uu]] <- unit;
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Flatten the remaining list structure
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Allocate "data frame" of right size
  unitSizes <- sapply(cdf, FUN=nrow);
  nbrOfCells <- sum(unitSizes);
  dataTypes <- sapply(cdf[[1]], FUN=storage.mode);
  df <- vector("list", length(dataTypes));
  names(df) <- names(dataTypes);
  for (key in names(df)) {
    df[[key]] <- vector(dataTypes[[key]], length=nbrOfCells);
  }

  # Copy values from the CDF list structure
  offset <- 0;
  while (length(cdf) > 0) {
    data <- cdf[[1]];
    nrow <- nrow(data);
    idxs <- offset + 1:nrow;
    for (key in names(df)) {
      df[[key]][idxs] <- data[[key]];
    }
    offset <- offset + nrow;
    cdf <- cdf[-1];
  }

  # Translate cell names
  names <- names(df);
  names <- sub("indices", "cell", names); 
  names <- sub("indexpos", "indexPos", names); 
  names(df) <- names;

  # Extract fields of interest
  df <- df[fields];

  # Make it a valid data frame
  if (drop && length(df) == 1) {
    df <- df[[1]];
  } else {
    attr(df, "row.names") <- .set_row_names(nbrOfCells);
    attr(df, "class") <- "data.frame";
  }

  df;
} # readCdfDataFrame()


# TO DO:
readCdfDataFrameFinal <- function(filename, units=NULL, groups=NULL, unitFields=NULL, groupFields=NULL, cellFields=NULL, verbose=0) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Argument 'filename':
  filename <- file.path(dirname(filename), basename(filename));
  if (!file.exists(filename))
    stop("File not found: ", filename);

  # Argument 'units':
  if (is.null(units)) {
  } else if (is.numeric(units)) {
    units <- as.integer(units);
    if (any(units < 1))
      stop("Argument 'units' contains non-positive indices.");
  } else {
    stop("Argument 'units' must be numeric or NULL: ", class(units)[1]);
  }

  # Argument 'groups':
  if (is.null(groups)) {
  } else if (is.numeric(groups)) {
    groups <- as.integer(groups);
    if (any(groups < 1))
      stop("Argument 'groups' contains non-positive indices.");
  } else {
    stop("Argument 'groups' must be numeric or NULL: ", class(groups)[1]);
  }

  # Argument 'unitFields':
  knownFields <- c("unit", "unitName", "unitDirection", "nbrOfUnitAtoms", "unitSize", "unitNumber", "unitType", "nbrOfGroups", "mutationType");
  
  # Argument 'groupFields':
  knownFields <- c("group", "groupName", "nbrOfGroupAtoms", "groupSize", "firstAtom", "lastAtom", "groupDirection");

  # Argument 'cellFields':
  knownFields <- c("cell", "x", "y", "probeSequence", "feat", "qual", "expos", "pos", "cbase", "pbase", "tbase", "atom", "index");


  # Argument 'verbose':
  if (length(verbose) != 1)
    stop("Argument 'verbose' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'verbose' must be an integer: ", verbose);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Call the C function
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
##  df <- .Call("R_affx_get_cdf_dataframe", filename, units, 
##               unitFields, groupFields, cellFields,
##               unitNames, groupNames,
##               verbose, PACKAGE="affxparser");

  df;
} # readCdfDataFrameFinal()



############################################################################
# HISTORY:
# 2008-03-24 [HB]
# o Created a readCdfDataFrame() that uses readCdf() to read the data and
#   then restructures it.  This will be used as a reference for the final
#   implementation.
# o Created a stub for readCdfDataFrame().
############################################################################  

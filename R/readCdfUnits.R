readCdfUnits <- function(filename, units=NULL, readXY=TRUE, readCells=FALSE, readBases=TRUE, readExpos=TRUE, readType=TRUE, readDirection=TRUE, stratifyBy=c("nothing", "pmmm", "pm", "mm"), verbose=0) {
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
    if (any(units < 0))
      stop("Argument 'units' contains negative indices.");
  } else {
    stop("Argument 'units' must be numeric or NULL: ", class(units)[1]);
  }

  # Argument 'units':
  if (length(verbose) != 1)
    stop("Argument 'units' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'units' must be an integer: ", verbose);

  # Argument 'readXY':
  readXY <- as.integer(as.logical(readXY));

  # Argument 'readCells':
  readCells <- as.integer(as.logical(readCells));

  # Argument 'readBases':
  readBases <- as.integer(as.logical(readBases));

  # Argument 'readExpos':
  readExpos <- as.integer(as.logical(readExpos));

  # Argument 'readType':
  readType <- as.integer(as.logical(readType));

  # Argument 'readDirection':
  readDirection <- as.integer(as.logical(readDirection));

  # Argument 'stratifyBy':
  stratifyBy <- match.arg(stratifyBy);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  cdf <- .Call("R_affx_get_cdf_units", filename, units, 
                readXY, readCells, readBases, readExpos, readType, 
                             readDirection, verbose, PACKAGE="affxparser");
  if (stratifyBy == "nothing")
    return(cdf);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Stratify by PM, MM, or PM & MM
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  isPm <- readCdfIsPm(filename, units=units);

  if (stratifyBy == "pmmm") {
    dimnames <- list(c("pm", "mm"), NULL);

    for (uu in seq(along=cdf)) {
      groups <- cdf[[uu]]$groups;
      for (gg in seq(along=groups)) {
        group <- groups[[gg]];
        pm <- isPm[[uu]][[gg]];
        mm <- which(!pm);
        pm <- which(pm);
        npm <- length(pm);
        if (npm != length(mm)) {
          # This is not expected to happen, but just in case.
          stop("Number of PM and MM probes differ in probeset #", uu,
                                     ": ", length(pm), " != ", length(mm));
        }
        pmmm <- .Internal(matrix(c(pm, mm), 2, npm, TRUE));
        dimnames(pmmm) <- dimnames;

        # Re-order cell elements according to PM/MM.
        dim <- c(2, ncol(pmmm));
        for (kk in seq(along=group)) {
          value <- group[[kk]][pmmm];
          dim(value) <- dim; 
          dimnames(value) <- dimnames;
          group[[kk]] <- value;
        }

#        group[["pmmm"]] <- pmmm;
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  } else if (stratifyBy == "pm") {
    for (uu in seq(along=cdf)) {
      groups <- cdf[[uu]]$groups;
      for (gg in seq(along=groups)) {
        group <- groups[[gg]];
        pm <- which(isPm[[uu]][[gg]]);
        for (kk in seq(along=group)) {
          group[[kk]] <- group[[kk]][pm];
        }
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  } else if (stratifyBy == "mm") {
    for (uu in seq(along=cdf)) {
      groups <- cdf[[uu]]$groups;
      for (gg in seq(along=groups)) {
        group <- groups[[gg]];
        mm <- which(!isPm[[uu]][[gg]]);
        for (kk in seq(along=group)) {
          group[[kk]] <- group[[kk]][mm];
        }
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  }

  cdf;
}


############################################################################
# HISTORY:
# 2006-02-21
# o Added argument 'readCells' to speed up the calculation of cell indices
#   from (x,y), i.e. cell = y * ncol + x.
# o Replaced argument 'splitPmMm' with 'stratifyBy'.  This will speed up
#   things down the stream.
# 2006-01-16
# o Added argument 'splitPmMm'. /HB
# 2006-01-09
# o Note that ../man/readCdfUnits.R in generated from the Rdoc comments
#   above.  See the R.oo package for details.  Don't remove the *.Rex files!
# o Created by HB.  The purpose was to make it possible to read subsets
#   of units and not just all units at once.  /HB
############################################################################  

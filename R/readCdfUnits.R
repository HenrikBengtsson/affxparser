readCdfUnits <- function(filename, units=NULL, readXY=TRUE, readBases=TRUE, readExpos=TRUE, readType=TRUE, readDirection=TRUE, stratifyBy=c("nothing", "pmmm", "pm", "mm"), readIndices=FALSE, verbose=0) {
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

  # Argument 'verbose':
  if (length(verbose) != 1)
    stop("Argument 'units' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'units' must be an integer: ", verbose);

  # Argument 'readXY':
  readXY <- as.integer(as.logical(readXY));

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

  # Argument 'readIndices':
  readIndices <- as.integer(as.logical(readIndices));


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  cdf <- .Call("R_affx_get_cdf_units", filename, units, 
                readXY, readBases, readExpos, 
                readType, readDirection, 
                readIndices, 
                verbose, PACKAGE="affxparser");

  if (stratifyBy == "nothing")
    return(cdf);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Stratify by PM, MM, or PM & MM
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  isPm <- readCdfIsPm(filename, units=units);

  # Using .subset2() instead of "[["() to avoid dispatching overhead etc.
  if (stratifyBy == "pmmm") {
    dimnames <- list(c("pm", "mm"), NULL);

    for (uu in seq(along=cdf)) {
#      groups <- cdf[[uu]]$groups;
      groups <- .subset2(.subset2(cdf, uu), "groups");
      ngroups <- length(groups);
      if (ngroups == 0)
        next;

      for (gg in 1:ngroups) {
#        group <- groups[[gg]];
        group <- .subset2(groups, gg);
#        pm <- isPm[[uu]][[gg]];
        pm <- .subset2(.subset2(isPm, uu), gg);
        idx <- 1:length(pm);
        mm <- idx[!pm];  # Note: which(!pm) is about 60% slower! /HB
        pm <- idx[pm];
        npm <- length(pm);
        if (npm != length(mm)) {
          # This is not expected to happen, but just in case.
          stop("Number of PM and MM probes differ in probeset #", uu,
                                     ": ", length(pm), " != ", length(mm));
        }
        pmmm <- .Internal(matrix(c(pm, mm), 2, npm, TRUE));
#        dimnames(pmmm) <- dimnames;

        # Re-order cell elements according to PM/MM.
        ngroup <- length(group);
        if (ngroup > 0) {
          dim <- c(2, npm);
          for (kk in 1:ngroup) {
  #          value <- group[[kk]][pmmm];
            value <- .subset(.subset2(group, kk), pmmm);
            dim(value) <- dim; 
            dimnames(value) <- dimnames;
            group[[kk]] <- value;
          }
        }

 #       group[["pmmm"]] <- pmmm;
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  } else if (stratifyBy == "pm") {
    for (uu in seq(along=cdf)) {
      groups <- cdf[[uu]]$groups;
      ngroups <- length(groups);
      if (ngroups == 0)
        next;

      for (gg in 1:ngroups) {
        group <- groups[[gg]];
        ngroup <- length(group);
        if (ngroup == 0)
          next;

        pm <- .subset2(.subset2(isPm, uu), gg);
        pm <- (1:length(pm))[pm]; # Note: which(!pm) is about 60% slower!
        for (kk in 1:ngroup) {
          group[[kk]] <- .subset(.subset2(group, kk), pm);
        }
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  } else if (stratifyBy == "mm") {
    for (uu in seq(along=cdf)) {
      groups <- cdf[[uu]]$groups;
      ngroups <- length(groups);
      if (ngroups == 0)
        next;

      for (gg in 1:ngroups) {
        group <- groups[[gg]];
        ngroup <- length(group);
        if (ngroup == 0)
          next;

        pm <- !.subset2(.subset2(isPm, uu), gg);
        mm <- (1:length(pm))[!pm]; # Note: which(!pm) is about 60% slower!
        for (kk in 1:ngroup) {
          group[[kk]] <- .subset(.subset2(group, kk), mm);
        }
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  }

  cdf;
} # readCdfUnits()


############################################################################
# HISTORY:
# 2006-03-28
# o Unit indices are now one-based. /HB
# o Renamed argument 'readCells' to 'readIndices'. /HB
# 2006-03-24
# o Not returning 'pmmm' field anymore.  A bit faster an smaller object.
# o Speed improvement of the "stratifyBy" code.  Instead of using which()
#   one can do the same oneself, which is 50% faster.  In addition, I have
#   replaced "[[" and "[" with .subset2() and .subset().
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


readCdfUnits <- function(filename, units=NULL, readXY=TRUE, readBases=TRUE, readExpos=TRUE, readType=TRUE, readDirection=TRUE, splitPmMm=FALSE, verbose=0) {
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

  # Argument 'readBases':
  readBases <- as.integer(as.logical(readBases));

  # Argument 'readExpos':
  readExpos <- as.integer(as.logical(readExpos));

  # Argument 'readType':
  readType <- as.integer(as.logical(readType));

  # Argument 'readDirection':
  readDirection <- as.integer(as.logical(readDirection));


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  cdf <- .Call("R_affx_get_cdf_units", filename, units, 
                readXY, readBases, readExpos, readType, readDirection,
                                            verbose, PACKAGE="affxparser");

  if (splitPmMm) {
    # Get PMs and MMs
    rownames <- c("PM", "MM");

    isPm <- readCdfIsPm(filename, units=units);
    for (uu in seq(along=cdf)) {
      groups <- cdf[[uu]]$groups;
      for (gg in seq(along=groups)) {
        group <- groups[[gg]];
        pm <- isPm[[uu]][[gg]];
        mm <- which(!pm);
        pm <- which(pm);
        if (length(pm) != length(mm)) {
          # This is not expected to happen, but just in case.
          stop("Number of PM and MM probes differ in probeset #", uu,
                                     ": ", length(pm), " != ", length(mm));
        }
        pmmm <- matrix(c(pm, mm), nrow=2, byrow=TRUE);
        rownames(pmmm) <- rownames;

        for (kk in seq(along=group)) {
          value <- matrix(group[[kk]][pmmm], nrow=2);
          rownames(value) <- rownames;
          group[[kk]] <- value;
        }

        group[["pmmm"]] <- pmmm;
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  }

  cdf;
}

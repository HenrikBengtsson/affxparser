readCdf <- function(filename, units=NULL, readXY=TRUE, readBases=TRUE,
                    readIndexpos=TRUE, readAtoms=TRUE,
                    readUnitType=TRUE, readUnitDirection=TRUE,
                    readUnitNumber=TRUE, readUnitAtomNumbers=TRUE,
                    readGroupAtomNumbers=TRUE, readGroupDirection=TRUE,
                    readIndices=FALSE, readIsPm=FALSE, 
                    stratifyBy=c("nothing", "pmmm", "pm", "mm"),
                    verbose=0) {
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Validate arguments
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    filename <- file.path(dirname(filename), basename(filename))
    if (!file.exists(filename))
        stop("File not found: ", filename)
    ## Argument 'units':
    if (! (is.null(units) || is.numeric(units))) {
        stop("Argument 'units' must be numeric or NULL: ", class(units)[1])
    }
    ## other arguments
    stratifyBy <- match.arg(stratifyBy)
    returnUnitType <- as.logical(readUnitType)
    returnUnitDirection <- as.logical(readUnitDirection)
    returnUnitAtomNumbers <- as.logical(readUnitAtomNumbers)
    returnUnitNumber <- as.logical(readUnitNumber)
    returnXY <- as.logical(readXY)
    returnIndices <- as.logical(readIndices)
    returnBases <- as.logical(readBases)
    returnAtoms <- as.logical(readAtoms)
    returnIndexpos <- as.logical(readIndexpos)
    returnIsPm <- as.logical(readIsPm)
    returnBlockDirection <- as.logical(readGroupDirection)
    returnBlockAtomNumbers <- as.logical(readGroupAtomNumbers)


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Read the CDF structure
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    cdf <- .Call("R_affx_get_cdf_file",
                 filename, as.integer(units), as.integer(verbose),
                 returnUnitType, returnUnitDirection,
                 returnUnitAtomNumbers, returnUnitNumber,
                 returnXY, returnIndices,
                 returnBases, returnAtoms, returnIndexpos, returnIsPm,
                 returnBlockDirection, returnBlockAtomNumbers,
                 PACKAGE="affxparser")
    if (stratifyBy == "nothing")
        return(cdf);


    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Stratify by PM/MM
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    isPm <- readCdfIsPm(filename, units=units);

    ## Using .subset2() instead of "[["() to avoid dispatching overhead etc.
    if (stratifyBy == "pmmm") {
        dimnames <- list(c("pm", "mm"), NULL);
        
        for (uu in seq(along=cdf)) {
            groups <- .subset2(.subset2(cdf, uu), "groups");
            ngroups <- length(groups);
            if (ngroups == 0)
                next;

            for (gg in 1:ngroups) {
                group <- .subset2(groups, gg);
                groupNoStratifyNames <- intersect(c("groupdirection",
                                                  "natoms", "ncellsperatom"),
                                                names(group))
                if(length(groupNoStratifyNames) > 0) {
                    groupNoStratifyElements <- group[groupNoStratifyNames]
                    group <- group[setdiff(names(group), c("groupdirection",
                                                  "natoms", "ncellsperatom"))]
                }
                pm <- .subset2(.subset2(isPm, uu), gg);
                idx <- 1:length(pm);
                mm <- idx[!pm];  # Note: which(!pm) is about 60% slower! /HB
                pm <- idx[pm];
                npm <- length(pm);
                if (npm != length(mm)) {
                    ## This is not expected to happen, but just in case.
                    stop("Number of PM and MM probes differ in probeset #", uu,
                         ": ", length(pm), " != ", length(mm));
                }
                pmmm <- matrix(c(pm, mm), nrow=2L, ncol=npm, byrow=TRUE);

                ## Re-order cell elements according to PM/MM.
                ngroup <- length(group);
                if (ngroup > 0) {
                    dim <- c(2, npm);
                    for (kk in 1:ngroup) {
                        ## value <- group[[kk]][pmmm];
                        value <- .subset(.subset2(group, kk), pmmm);
                        dim(value) <- dim; 
                        dimnames(value) <- dimnames;
                        group[[kk]] <- value;
                    }
                }
                
                if(length(groupNoStratifyNames) > 0) {
                    group <- c(group, groupNoStratifyElements)
                }
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
                groupNoStratifyNames <- intersect(c("groupdirection",
                                                  "natoms", "ncellsperatom"),
                                                names(group))
                if(length(groupNoStratifyNames) > 0) {
                    groupNoStratifyElements <- group[groupNoStratifyNames]
                    group <- group[setdiff(names(group), c("groupdirection",
                                                  "natoms", "ncellsperatom"))]
                }
                ngroup <- length(group);
                if (ngroup == 0)
                    next;
                
                pm <- .subset2(.subset2(isPm, uu), gg);
                pm <- (1:length(pm))[pm]; # Note: which(!pm) is about 60% slower!
                for (kk in 1:ngroup) {
                    group[[kk]] <- .subset(.subset2(group, kk), pm);
                }
                if(length(groupNoStratifyNames) > 0) {
                    group <- c(group, groupNoStratifyElements)
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
                groupNoStratifyNames <- intersect(c("groupdirection",
                                                  "natoms", "ncellsperatom"),
                                                names(group))
                if(length(groupNoStratifyNames) > 0) {
                    groupNoStratifyElements <- group[groupNoStratifyNames]
                    group <- group[setdiff(names(group), c("groupdirection",
                                                  "natoms", "ncellsperatom"))]
                }
                ngroup <- length(group);
                if (ngroup == 0)
                    next;
                
                pm <- .subset2(.subset2(isPm, uu), gg);
                mm <- (1:length(pm))[!pm]; # Note: which(!pm) is about 60% slower!
                for (kk in 1:ngroup) {
                    group[[kk]] <- .subset(.subset2(group, kk), mm);
                }
                if(length(groupNoStratifyNames) > 0) {
                    group <- c(group, groupNoStratifyElements)
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
# 2010-12-12
# o ROBUSTNESS: Replaces .Internal(matrix(...)) with matrix().
#   In the upcoming R 2.13.0 matrix() has less overhead.
############################################################################

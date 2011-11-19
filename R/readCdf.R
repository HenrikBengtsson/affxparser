#########################################################################/**
# @RdocFunction readCdf
#
# @title "Parsing a CDF file using Affymetrix Fusion SDK"
#
# \description{
#  @get "title". 
#  This function parses a CDF file using the Affymetrix Fusion SDK.
#  \emph{This function will most likely be replaced by the more
#        general \code{\link{readCdfUnits}()} function.}
# }
# 
# \usage{
#  readCdf(filename, units=NULL, 
#          readXY=TRUE, readBases=TRUE, 
#          readIndexpos=TRUE, readAtoms=TRUE, 
#          readUnitType=TRUE, readUnitDirection=TRUE, 
#          readUnitNumber=TRUE, readUnitAtomNumbers=TRUE, 
#          readGroupAtomNumbers=TRUE, readGroupDirection=TRUE, 
#          readIndices=FALSE, readIsPm=FALSE, 
#          stratifyBy=c("nothing", "pmmm", "pm", "mm"), 
#          verbose=0)
# }
# 
# \arguments{
#  \item{filename}{The filename of the CDF file.}
#  \item{units}{An @integer @vector of unit indices
#    specifying which units to be read.  If @NULL, all units are read.}
#  \item{readXY}{If @TRUE, cell row and column (x,y) coordinates are
#     retrieved, otherwise not.}
#  \item{readBases}{If @TRUE, cell P and T bases are retrieved, otherwise not.}
#  \item{readIndexpos}{If @TRUE, cell indexpos are retrieved, otherwise not.}
#  \item{readExpos}{If @TRUE, cell "expos" values are retrieved, otherwise not.}
#  \item{readUnitType}{If @TRUE, unit types are retrieved, otherwise not.}
#  \item{readUnitDirection}{If @TRUE, unit directions are retrieved, otherwise not.}
#  \item{readUnitNumber}{If @TRUE, unit numbers are retrieved, otherwise not.}
#  \item{readUnitAtomNumbers}{If @TRUE, unit atom numbers are retrieved, otherwise not.}
#  \item{readGroupAtomNumbers}{If @TRUE, group atom numbers are retrieved, otherwise not.}
#  \item{readGroupDirection}{If @TRUE, group directions are retrieved, otherwise not.}
#  \item{readIndices}{If @TRUE, cell indices \emph{calculated} from
#    the row and column (x,y) coordinates are retrieved, otherwise not.
#     Note that these indices are \emph{one-based}.}
#  \item{readIsPm}{If @TRUE, cell flags indicating whether the cell
#    is a perfect-match (PM) probe or not are retrieved, otherwise not.}
#  \item{stratifyBy}{A @character string specifying which and how
#    elements in group fields are returned.
#    If \code{"nothing"}, elements are returned as is, i.e. as @vectors.
#    If \code{"pm"}/\code{"mm"}, only elements corresponding to 
#    perfect-match (PM) / mismatch (MM) probes are returned (as @vectors).
#    If \code{"pmmm"}, elements are returned as a matrix where the
#    first row holds elements corresponding to PM probes and the second
#    corresponding to MM probes.  Note that in this case, it is assumed 
#    that there are equal number of PMs and MMs; if not, an error is
#    generated.  
#    Moreover, the PMs and MMs may not even be paired, i.e. there is no 
#    guarantee that the two elements in a column corresponds to a 
#    PM-MM pair.}
#  \item{verbose}{An @integer specifying the verbose level. If 0, the
#    file is parsed quietly.  The higher numbers, the more details.}
# }
# 
# \value{
#   A list with one component for each unit. Every component is again a
#   list with three components    
#   \item{groups}{This is again a list with one component for each group
#     (also called block). The information on each group is a list with 5
#     components, \code{x}, \code{y}, \code{pbase}, \code{tbase},
#   \code{expos}.}
#   \item{type}{type of the unit.}
#   \item{direction}{direction of the unit.}
# }
#
# \note{
#   This version of the function does not return information on the QC
#   probes. This will be added in a (near) future release. In addition we
#   expect the header to be part of the returned object.
# 
#   So expect changes to the structure of the value of the function in
#   next release. Please contact the developers for details.
# }
#
# \section{Cell indices are one-based}{
#   Note that in \pkg{affxparser} all \emph{cell indices} are by 
#   convention \emph{one-based}, which is more convenient to work 
#   with in \R.  For more details on one-based indices, see
#   @see "2. Cell coordinates and cell indices".
# }
# 
# \author{
#  James Bullard, \email{bullard@stat.berkeley.edu} and
#  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}.
# }
# 
# \seealso{
#   It is recommended to use @see "readCdfUnits" instead of this method.
#   @see "readCdfHeader" for getting the header of a CDF file.
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
# @keyword "internal"
#*/#########################################################################  
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

    # Sanity check
    if (is.null(cdf)) {
      stop("Failed to read CDF file: ", filename);
    }

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
} # readCdf()


############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2011-02-15
# o DOCUMENTATION: Converted the Rd to an Rdoc comment in this file.
# 2010-12-12
# o ROBUSTNESS: Replaces .Internal(matrix(...)) with matrix().
#   In the upcoming R 2.13.0 matrix() has less overhead.
############################################################################

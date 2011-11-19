#########################################################################/**
# @RdocFunction readCdfUnits
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
#  \item{readXY}{If @TRUE, cell row and column (x,y) coordinates are
#     retrieved, otherwise not.}
#  \item{readBases}{If @TRUE, cell P and T bases are retrieved, otherwise not.}
#  \item{readExpos}{If @TRUE, cell "expos" values are retrieved, otherwise not.}
#  \item{readType}{If @TRUE, unit types are retrieved, otherwise not.}
#  \item{readDirection}{If @TRUE, unit \emph{and} group directions are
#    retrieved, otherwise not.}
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
#  \item{readIndices}{If @TRUE, cell indices \emph{calculated} from
#    the row and column (x,y) coordinates are retrieved, otherwise not.
#     Note that these indices are \emph{one-based}.}
#  \item{verbose}{An @integer specifying the verbose level. If 0, the
#    file is parsed quietly.  The higher numbers, the more details.}
# }
# 
# \value{
#  A named @list where the names corresponds to the names
#  of the units read.  Each element of the list is in turn a
#  @list structure with three components:
#  \item{groups}{A @list with one component for each group 
#   (also called block). The information on each group is a 
#   @list of up to seven components: \code{x}, \code{y}, 
#   \code{pbase}, \code{tbase}, \code{expos}, \code{indices},
#   and \code{direction}.
#   All fields but the latter have the same number of values as 
#   there are cells in the group.  The latter field has only
#   one value indicating the direction for the whole group.
#  }
#  \item{type}{An @integer specifying the type of the
#    unit, where 1 is "expression", 2 is "genotyping", 3 is "CustomSeq", 
#    and 4 "tag".}
#  \item{direction}{An @integer specifying the direction
#    of the unit, which defines if the probes are interrogating the sense
#    or the anti-sense target, where 0 is "no direction", 1 is "sense", and
#    2 is "anti-sense".}
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
#  James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
#  Daniel Hansen, \email{khansen@stat.berkeley.edu}.
#  Modified by Henrik Bengtsson (\url{http://www.braju.com/R/}) to read
#  any subset of units and/or subset of parameters, to stratify by PM/MM,
#  and to return cell indices.d
# }
# 
# @examples "../incl/readCdfUnits.Rex"
# 
# \seealso{
#   @see "readCdfCellIndices".
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
    stop("Argument 'verbose' must be a single integer.");
  verbose <- as.integer(verbose);
  if (!is.finite(verbose))
    stop("Argument 'verbose' must be an integer: ", verbose);



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

  # Sanity check
  if (is.null(cdf)) {
    stop("Failed to read CDF file: ", filename);
  }

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
        pmmm <- matrix(c(pm, mm), nrow=2L, ncol=npm, byrow=TRUE);
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

        pm <- .subset2(.subset2(isPm, uu), gg);
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
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2011-02-15
# o DOCUMENTATION: Clarified in help(readCdfUnits) that (x,y) coordinates
#   are zero-based and the _from (x,y) calculated_ cell indices are 
#   one-based, regardless what the indices on file are.
# 2010-12-12
# o ROBUSTNESS: Replaces .Internal(matrix(...)) with matrix().
#   In the upcoming R 2.13.0 matrix() has less overhead.
# 2006-12-30
# o Now 'readDirection=TRUE' also return group directions.
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

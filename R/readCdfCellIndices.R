#########################################################################/**
# @RdocFunction readCdfCellIndices
# 
# @title "Reads (one-based) cell indices of units (probesets) in an Affymetrix CDF file"
# 
# @synopsis
# 
# \description{
#   @get "title".
# }
# 
# \arguments{
#   \item{filename}{The filename of the CDF file.}
#   \item{units}{An @integer @vector of unit indices
#     specifying which units to be read.  If @NULL, all units are read.}
#   \item{stratifyBy}{A \code{\link[base]{character}} string specifying which and how
#     elements in group fields are returned.
#     If \code{"nothing"}, elements are returned as is, i.e. as @vectors.
#     If \code{"pm"}/\code{"mm"}, only elements corresponding to 
#     perfect-match (PM) / mismatch (MM) probes are returned (as @vectors).
#     If \code{"pmmm"}, elements are returned as a matrix where the
#     first row holds elements corresponding to PM probes and the second
#     corresponding to MM probes.  Note that in this case, it is assumed 
#     that there are equal number of PMs and MMs; if not, an error is
#     generated.  
#     Moreover, the PMs and MMs may not even be paired, i.e. there is no 
#     guarantee that the two elements in a column corresponds to a 
#     PM-MM pair.}
#   \item{verbose}{An @integer specifying the verbose level. If 0, the
#     file is parsed quietly.  The higher numbers, the more details.}
# }
# 
# \value{
#   A named @list where the names corresponds to the names
#   of the units read.  Each unit element of the list is in turn a
#   @list structure with one element \code{groups} which in turn
#   is a @list.  Each group element in \code{groups} is a @list
#   with a single field named \code{indices}.  Thus, the structure is
#   \preformatted{
#     cdf
#      +- unit #1
#      |   +- "groups"
#      |       +- group #1
#      |       |   +- "indices"
#      |       |  group #2
#      |       |   +- "indices"
#      |       .  
#      |       +- group #K
#      |           +- "indices"
#      +- unit #2
#      .
#      +- unit #J
#   }
#   
#   This is structure is compatible with what @see "readCdfUnits" returns.
#
#   Note that these indices are \emph{one-based}.
# }
#
# \section{Cell indices are one-based}{
#   Note that in \pkg{affxparser} all \emph{cell indices} are by 
#   convention \emph{one-based}, which is more convenient to work 
#   with in \R.  For more details on one-based indices, see
#   @see "2. Cell coordinates and cell indices".
# }
# 
# @author "HB"
# 
#  \seealso{
#    @see "readCdfUnits".
#  }
# 
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCdfCellIndices <- function(filename, units=NULL, stratifyBy=c("nothing", "pmmm", "pm", "mm"), verbose=0) {
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

  # Argument 'stratifyBy':
  stratifyBy <- match.arg(stratifyBy);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  cdf <- .Call("R_affx_get_cdf_cell_indices", filename, units, verbose,
                                                     PACKAGE="affxparser");

  # Sanity check
  if (is.null(cdf)) {
    stop("Failed to read cell indices from CDF file: ", filename);
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
        dim <- c(2, npm);
#          value <- group[[1]][pmmm];
        value <- .subset(.subset2(group, 1), pmmm);
        dim(value) <- dim; 
        dimnames(value) <- dimnames;
        group[[1]] <- value;

 #       group[["pmmm"]] <- pmmm;
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  } else if (stratifyBy == "pm") {
    for (uu in seq(along=cdf)) {
#      groups <- cdf[[uu]]$groups;
      groups <- .subset2(.subset2(cdf, uu), "groups");
      ngroups <- length(groups);
      if (ngroups == 0)
        next;

      for (gg in 1:ngroups) {
#        group <- groups[[gg]];
        group <- .subset2(groups, gg);
        ngroup <- length(group);
        if (ngroup == 0)
          next;

        pm <- .subset2(.subset2(isPm, uu), gg);
        pm <- (1:length(pm))[pm]; # Note: which(pm) is about 60% slower!
        for (kk in 1:ngroup) {
          group[[kk]] <- .subset(.subset2(group, kk), pm);
        }
        groups[[gg]] <- group;
      } # for (gg ...)
      cdf[[uu]]$groups <- groups;
    } # for (uu ...)
  } else if (stratifyBy == "mm") {
    for (uu in seq(along=cdf)) {
#      groups <- cdf[[uu]]$groups;
      groups <- .subset2(.subset2(cdf, uu), "groups");
      ngroups <- length(groups);
      if (ngroups == 0)
        next;

      for (gg in 1:ngroups) {
#        group <- groups[[gg]];
        group <- .subset2(groups, gg);
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
} # readCdfUnitInidices()



############################################################################
# HISTORY:
# 2011-11-18
# o ROBUSTNESS: Added sanity check that the native code did not return NULL.
# 2010-12-12
# o ROBUSTNESS: Replaces .Internal(matrix(...)) with matrix().
#   In the upcoming R 2.13.0 matrix() has less overhead.
# 2006-12-10
# o BUG FIX: Same stratifyBy="mm" bug here as in readCdfUnits().
# 2006-07-22
# o Making more use of .subset2().
# 2006-05-20
# o Rd fix: The \value{} was incorrect.
# 2006-05-12
# o Removed argument 'flat'.  Will not be used for a while.  The intention
#   was to remove the redundant levels of "groups" and possibly also the
#   "indices" level.  That would most likely speed up things a bit, but it
#   would also require that readCelUnits() understand this other format too.
# 2006-04-01
# o Created, because it is very commonly used and is about 5 times faster
#   than using readCdfUnits(..., readIndices=TRUE).  /HB
############################################################################  

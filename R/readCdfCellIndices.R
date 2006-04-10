#########################################################################/**
# @RdocFunction readCdfCellIndices
# 
# @title "Reads cell indices of units (probesets) in an Affymetrix CDF file"
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
#   of the units read.  Each element of the list is in turn a
#   @list structure with three components:
#   \item{groups}{A @list with one component for each group 
#    (also called block). The information on each group is a 
#    @list with five components: \code{x}, \code{y}, 
#    \code{pbase}, \code{tbase}, \code{expos}, and \code{indices}.}
#   \item{type}{An @integer specifying the type of the
#     unit, where 1 is "expression", 2 is "genotyping", 3 is "CustomSeq", 
#     and 4 "tag".}
#   \item{direction}{An @integer specifying the direction
#     of the unit, which defines if the probes are interrogating the sense
#     or the anti-sense target, where 0 is "no direction", 1 is "sense", and
#     2 is "anti-sense".}
# }
# 
# @author
# 
#  \seealso{
#    @see "readCdfUnits".
#  }
# 
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
readCdfCellIndices <- function(filename, units=NULL, stratifyBy=c("nothing", "pmmm", "pm", "mm"), flat=FALSE, verbose=0) {
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

  # Argument 'stratifyBy':
  stratifyBy <- match.arg(stratifyBy);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Read the CDF file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  cdf <- .Call("R_affx_get_cdf_cell_indices", filename, units, verbose,
                                                     PACKAGE="affxparser");

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
} # readCdfUnitInidices()


############################################################################
# HISTORY:
# 2006-04-01
# o Created, because it is very commonly used and is about 5 times faster
#   than using readCdfUnits(..., readIndices=TRUE).  /HB
############################################################################  

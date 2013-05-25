#########################################################################/**
# @RdocFunction updateCelUnits
#
# @title "Updates a CEL file unit by unit"
#
# @synopsis
#
# \description{
#   @get "title".\cr
#
#   \emph{Please note that, contrary to @see "readCelUnits", this method
#   can only update a single CEL file at the time.}
# }
#
# \arguments{
#   \item{filename}{The filename of the CEL file.}
#   \item{cdf}{A (optional) CDF @list structure either with
#     field \code{indices} or fields \code{x} and \code{y}.
#     If @NULL, the unit names (and from there the cell indices) are
#     inferred from the names of the elements in \code{data}.
#   }
#   \item{data}{A @list structure in a format similar to what is returned
#      by @see "readCelUnits" for \emph{a single CEL file only}.}
#   \item{...}{Optional arguments passed to @see "readCdfCellIndices",
#      which is called if \code{cdf} is not given.}
#   \item{verbose}{An @integer specifying how much verbose details are
#     outputted.}
# }
#
# \value{
#   Returns what @see "updateCel" returns.
# }
#
# \section{Working with re-arranged CDF structures}{
#   Note that if the \code{cdf} structure is specified the CDF file is
#   \emph{not} queried, but all information about cell x and y locations,
#   that is, cell indices is expected to be in this structure.  This can
#   be very useful when one work with a cdf structure that originates
#   from the underlying CDF file, but has been restructured for instance
#   through the @see "applyCdfGroups" method, and \code{data}
#   correspondingly.  This update method knows how to update such
#   structures too.
# }
#
# @examples "../incl/updateCelUnits.Rex"
#
# @author "HB"
#
# \seealso{
#   Internally, @see "updateCel" is used.
# }
#
# @keyword "file"
# @keyword "IO"
#*/#########################################################################
updateCelUnits <- function(filename, cdf=NULL, data, ..., verbose=0) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'data':
  if (is.list(data)) {
    # For now, assume a correct cel structure
  } else {
    stop("Argument 'data' must be a list: ", mode(data));
  }

  # Argument 'cdf':
  if (is.list(cdf)) {
  } else if (!is.null(cdf)) {
    stop("Argument 'cdf' must be a list or NULL: ", mode(cdf));
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Find out what cell indices are to be written
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (is.null(cdf)) {
    # No cell indices were given. Thus, pick the unit names from the 'data'
    # object and identify the unit indices.  Then, read the cell indices
    # for these units.
    # i. Get the chip type
    chipType <- readCelHeader(filename)$chiptype;
    # ii. Find the CDF file
    cdfFile <- findCdf(chipType);
    # iii. Read all unit names
    unitNames <- readCdfUnitNames(cdfFile); # CRASHES AFTER MULTIPLE CALLS!
    # iv. Map unit names to unit indices
    units <- match(names(data), unitNames);
    # v. Validate
    if (any(is.na(units))) {
      stop("Could not identify unit indices. Some unit names in argument 'data' do not exist in the CDF for '", chipType, "'.");
    }
    cdf <- readCdfCellIndices(cdfFile, units=units, ...);
    # Confirmed: 2006-08-22
  } else if (is.list(cdf)) {
    # Reformat 'cdf' to only contain cell 'indices'
    unit <- cdf[[1]];
    groups <- unit$groups;
    group <- groups[[1]];
    if ("indices" %in% names(group)) {
      if (!identical(names(group), "indices")) {
        cdf <- applyCdfGroups(cdf, FUN=cdfGetFields, fields="indices");
      }
    } else if (all(c("x", "y") %in% names(group))) {
      ncol <- readCelHeader(filename)$cols;
      cdf <- applyCdfGroups(cdf, FUN=function(groups) {
        lapply(groups, FUN=function(group) {
          list(indices=as.integer(group$y*ncol+group$x+1));
        })
      })
    } else {
      stop("Argument 'cdf' must be a CDF structure with group fields 'indices' or 'x' & 'y': ", paste(names(group), collapse=", "));
    }
  }

  # For now, assume the 'cdf' contains cell 'indices' only.
  indices <- unlist(cdf, use.names=FALSE);

  ncells <- length(indices);
#  cat(sprintf("Number of cells to be updated: %d\n", ncells));

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Investigate the 'data' list to see what fields are included.
  # We do this under the assumption that all units have the same
  # structure.
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  group <- data[[1]][[1]];
  fields <- names(group);
  # Pull out the fields that can be written to a CEL file
  fields <- intersect(fields, c("intensities", "stdvs", "pixels"));
  group <- NULL; # Not needed anymore

#  cat(sprintf("Fields to be updated: %s\n", paste(fields, collapse=", ")));

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Extract the data for each of these fields (one by one).
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  values <- list();
  for (field in fields) {
    tmp <- lapply(data, lapply, .subset2, field);
    values[[field]] <- unlist(tmp, use.names=FALSE);
    tmp <- NULL; # Not needed anymore
  }

  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Update CEL file
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  updateCel(filename, indices=indices, intensities=values$intensities, stdvs=values$stdvs, pixels=values$pixels);
} # updateCelUnits()

############################################################################
# HISTORY:
# 2006-08-22
# o There is some weird memory bug in at least Windows that makes R crash
#   on multiple (10-20) subsequent calls to readCdfNnn().  Have been
#   troubleshooting for a more than a day, but I can't find why it is.
#   Email HB for details.
# 2006-08-21
# o First test passed.
# o Created.
############################################################################

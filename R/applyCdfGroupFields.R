########################################################################/**
# @RdocFunction applyCdfGroupFields
#
# @title "Applies a function to a list of fields of each group in a CDF structure"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{cdf}{A CDF @list structure.}
#  \item{fcn}{A @function that takes a @list structure of fields
#             and returns an updated @list of fields.}
#  \item{...}{Arguments passed to the \code{fcn} function.}
# }
#
# \value{
#  Returns an updated CDF @list structure.
# }
#
# @author "HB"
#
# \seealso{
#   @see "applyCdfGroups".
# }
#
# @keyword programming
#**/####################################################################### 
applyCdfGroupFields <- function(cdf, fcn, ...) {
  applyCdfGroups(cdf, function(groups) {
    lapply(groups, FUN=fcn, ...);
  });
} # applyCdfGroupFields()



############################################################################
# HISTORY:
# 2009-05-29
# o Added Rdoc comments.
# o Created.
############################################################################

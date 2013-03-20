########################################################################/**
# @RdocFunction cdfGetGroups
#
# @title "Gets a subset of groups in a CDF structure"
#
# \description{
#  @get "title".
#
#  This @function is designed to be used with @see "applyCdfGroups".
# }
#
# @synopsis
#
# \arguments{
#  \item{groups}{A @list of groups.}
#  \item{which}{An @integer or @character @vector of groups be returned.}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure of groups.
# }
#
# \seealso{
#  @see "applyCdfGroups".
# }
#
# @author "HB"
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfGetGroups <- function(groups, which, ...) {
  .subset(groups, which);
}


############################################################################
# HISTORY:
# 2006-04-24
# o Created.
############################################################################  

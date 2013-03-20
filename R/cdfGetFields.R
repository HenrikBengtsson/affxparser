########################################################################/**
# @RdocFunction cdfGetFields
#
# @title "Gets a subset of groups fields in a CDF structure"
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
#  \item{fields}{A @character @vector of names of fields to be returned.}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns a @list structure of groups.
# }
#
# \details{
#   Note that an error is \emph{not} generated for missing fields.  
#   Instead the field is returned with value @NA.  The reason for this
#   is that it is much faster.
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
cdfGetFields <- function(groups, fields, ...) {
  lapply(groups, function(group) .subset(group, fields))
}

############################################################################
# HISTORY:
# 2006-03-21 (Stockholm, Sveavägen)
# o Created.
############################################################################  

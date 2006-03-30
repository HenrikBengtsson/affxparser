########################################################################/**
# @RdocFunction cdfGetFields
#
# @title "Gets a subset of groups fields in a CDF structure"
#
# \description{
#  @get "title".
#
#  This @function is design to be used with @see "applyCdfGroups"
#  on an Affymetrix Mapping (SNP) CDF @list structure.
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
# @author
#
# @keyword programming
#**/#######################################################################
cdfGetFields <- function(groups, fields, ...) {
  lapply(groups, function(group) group[fields])
}

############################################################################
# HISTORY:
# 2006-03-21 (Stockholm, Sveavägen)
# o Created.
############################################################################  

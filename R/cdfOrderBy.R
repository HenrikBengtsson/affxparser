########################################################################/**
# @RdocFunction cdfOrderBy
#
# @title "Orders the fields according to the value of another field in the same CDF group"
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
#  \item{field}{The field whose values are used to order the other fields.}
#  \item{...}{Optional arguments passed @see "base::order".}
# }
#
# \value{
#  Returns a @list structure of groups.
# }
#
# \seealso{
#  @see "cdfOrderColumnsBy".
#  @see "applyCdfGroups".
# }
#
# @author "HB"
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfOrderBy <- function(groups, field, ...) {
  lapply(groups, function(group) {
    o <- order(.subset2(group, field), ...);
    lapply(group, FUN=.subset, o);
  })
}

############################################################################
# HISTORY:
# 2006-04-20
# o Created.
############################################################################  

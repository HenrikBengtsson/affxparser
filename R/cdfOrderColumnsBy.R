########################################################################/**
# @RdocFunction cdfOrderColumnsBy
#
# @title "Orders the columns of fields according to the values in a certain row of another field in the same CDF group"
#
# \description{
#  @get "title".
#  Note that this method requires that the group fields are matrices.
#  
#  This @function is design to be used with @see "applyCdfGroups"
#  on an Affymetrix Mapping (SNP) CDF @list structure.
# }
#
# @synopsis
#
# \arguments{
#  \item{groups}{A @list of groups.}
#  \item{field}{The field whose values in row \code{row} are used to order 
#     the other fields.}
#  \item{row}{The row of the above field to be used to find the order.}
#  \item{...}{Optional arguments passed @see "base::order".}
# }
#
# \value{
#  Returns a @list structure of groups.
# }
#
# \seealso{
#  @see "cdfOrderBy".
#  @see "applyCdfGroups".
# }
#
# @author "HB"
#
# @keyword programming
# @keyword internal
#**/#######################################################################
cdfOrderColumnsBy <- function(groups, field, row=1, ...) {
  lapply(groups, function(group) {
    occ <- order(.subset2(group, field)[row,], ...);
    lapply(group, FUN=function(field) {
      nrow <- .subset(dim(field), 1);
      if (nrow > 0) {
        .subset(field, 1:nrow, occ);
      } else {
        field;
      }
    })
  })
}

############################################################################
# HISTORY:
# 2006-04-20
# o Created.
############################################################################  

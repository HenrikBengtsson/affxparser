%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  cdfOrderColumnsBy.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{cdfOrderColumnsBy}
\alias{cdfOrderColumnsBy}


 \title{Orders the columns of fields according to the values in a certain row of another field in the same CDF group}

 \description{
  Orders the columns of fields according to the values in a certain row of another field in the same CDF group.
  Note that this method requires that the group fields are matrices.

  This \code{\link[base]{function}} is design to be used with \code{\link{applyCdfGroups}}()
  on an Affymetrix Mapping (SNP) CDF \code{\link[base]{list}} structure.
 }

 \usage{
cdfOrderColumnsBy(groups, field, row=1, ...)
}

 \arguments{
  \item{groups}{A \code{\link[base]{list}} of groups.}
  \item{field}{The field whose values in row \code{row} are used to order
     the other fields.}
  \item{row}{The row of the above field to be used to find the order.}
  \item{...}{Optional arguments passed \code{\link[base]{order}}().}
 }

 \value{
  Returns a \code{\link[base]{list}} structure of groups.
 }

 \seealso{
  \code{\link{cdfOrderBy}}().
  \code{\link{applyCdfGroups}}().
 }

 \author{Henrik Bengtsson}



\keyword{programming}
\keyword{internal}

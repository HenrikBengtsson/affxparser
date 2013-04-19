%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  writeCelHeader.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{writeCelHeader}
\alias{writeCelHeader}


 \title{Writes a CEL header to a connection}

 \usage{
writeCelHeader(con, header, outputVersion=c("4"), ...)
}

 \description{
   Writes a CEL header to a connection.
 }

 \arguments{
   \item{con}{A \code{\link[base:connections]{connection}}.}
   \item{header}{A \code{\link[base]{list}} structure describing the CEL header, similar
     to the structure returned by \code{\link{readCelHeader}}().}
   \item{outputFormat}{A \code{\link[base]{character}} string specifying the output format.
     Currently only CEL version 4 (binary;XDA) are supported.}
   \item{...}{Not used.}
 }

 \value{
   Returns (invisibly) the pathname of the file created.
 }

 \details{
   Currently only CEL version 4 (binary;XDA) headers can be written.
 }

 \section{Redundant fields}{
   The CEL v4 header contains redundant information. To avoid inconsistency
   this method generates such redundant values from the original values.
   This is consistent to how the CEL reader in Fusion SDK does it, cf.
   \code{\link{readCelHeader}}().  The redundant information is in the (CEL v3)
   \code{header} field, which contains the CEL header information as it
   would appear in the CEL v3 format.  This in turn contains a DAT header
   field reproducing the DAT header from the image analysis.  It is from
   this DAT header that the chip type is extracted.
 }

 \author{Henrik Bengtsson}




\keyword{file}
\keyword{IO}
\keyword{internal}

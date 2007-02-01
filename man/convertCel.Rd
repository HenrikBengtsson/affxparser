%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  convertCel.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{convertCel}
\alias{convertCel}


 \title{Converts a CEL into the same CEL but with another format}

 \usage{convertCel(filename, outFilename, readMap=NULL, writeMap=NULL, version="4", ..., .validate=FALSE, verbose=FALSE)}

 \description{
   Converts a CEL into the same CEL but with another format.
   Currently only CEL files in version 4 (binary/XDA) can be written.
   However, any input format is recognized.
 }

 \arguments{
   \item{filename}{The pathname of the original CEL file.}
   \item{outFilename}{The pathname of the destination CEL file.
     If the same as the source file, an exception is thrown.}
   \item{readMap}{An optional read map for the input CEL file.}
   \item{writeMap}{An optional write map for the output CEL file.}
   \item{version}{The version of the output file format.}
   \item{...}{Not used.}
   \item{.validate}{If \code{\link[base:logical]{TRUE}}, a consistency test between the generated
     and the original CDF is performed.  Note that the memory overhead
     for this can be quite large, because two complete CDF structures
     are kept in memory at the same time.}
   \item{verbose}{If \code{\link[base:logical]{TRUE}}, extra details are written while processing.}
 }

 \value{
   Returns (invisibly) \code{\link[base:logical]{TRUE}} if a new CEL was generated, otherwise \code{\link[base:logical]{FALSE}}.
 }

 \section{Benchmarking of ASCII and binary CELs}{
   Binary CELs are much faster to read than ASCII CELs.  Here are some
   example for reading complete CELs (the differnce is even larger when
   reading CELs in subsets):
   \itemize{
     \item To do
   }
 }

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}

 \seealso{
   \code{\link{createCel}}().
 }



\keyword{file}
\keyword{IO}

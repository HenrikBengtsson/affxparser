%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  convertCdf.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{convertCdf}
\alias{convertCdf}


 \title{Converts a CDF into the same CDF but with another format}

 \usage{
convertCdf(filename, outFilename, version="4", force=FALSE, ..., .validate=TRUE,
  verbose=FALSE)
}

 \description{
   Converts a CDF into the same CDF but with another format.
   Currently only CDF files in version 4 (binary/XDA) can be written.
   However, any input format is recognized.
 }

 \arguments{
   \item{filename}{The pathname of the original CDF file.}
   \item{outFilename}{The pathname of the destination CDF file.
     If the same as the source file, an exception is thrown.}
   \item{version}{The version of the output file format.}
   \item{force}{If \code{\link[base:logical]{FALSE}}, and the version of the orignal CDF is the
     same as the output version, the new CDF will not be generated,
     otherwise it will.}
   \item{...}{Not used.}
   \item{.validate}{If \code{\link[base:logical]{TRUE}}, a consistency test between the generated
     and the original CDF is performed.  Note that the memory overhead
     for this can be quite large, because two complete CDF structures
     are kept in memory at the same time.}
   \item{verbose}{If \code{\link[base:logical]{TRUE}}, extra details are written while processing.}
 }

 \value{
   Returns (invisibly) \code{\link[base:logical]{TRUE}} if a new CDF was generated, otherwise \code{\link[base:logical]{FALSE}}.
 }

 \section{Benchmarking of ASCII and binary CDFs}{
   Binary CDFs are much faster to read than ASCII CDFs.  Here are some
   example for reading complete CDFs (the differnce is even larger when
   reading CDFs in subsets):
   \itemize{
     \item HG-U133A (22283 units): ASCII 11.7s (9.3x), binary 1.20s (1x).
     \item Hu6800 (7129 units): ASCII 3.5s (6.1x), binary 0.57s (1x).
   }
 }

 \section{Confirmed convertions to binary (XDA) CDFs}{
   The following chip types have been converted using \code{convertCdf()}
   and then verified for correctness using \code{compareCdfs()}:
   ASCII-to-binary: HG-U133A, Hu6800.
   Binary-to-binary: Test3.
 }

 \examples{
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################


chipType <- "Test3"
cdfFiles <- findCdf(chipType, firstOnly=FALSE)
cdfFiles <- list(
  ASCII=grep("ASCII", cdfFiles, value=TRUE),
  XDA=grep("XDA", cdfFiles, value=TRUE)
)

outFile <- file.path(tempdir(), sprintf("\%s.cdf", chipType))
convertCdf(cdfFiles$ASCII, outFile, verbose=TRUE)

##############################################################
}                                                     # STOP #
##############################################################


}

 \author{Henrik Bengtsson}

 \seealso{
   See \code{\link{compareCdfs}}() to compare two CDF files.
   \code{\link{writeCdf}}().
 }



\keyword{file}
\keyword{IO}

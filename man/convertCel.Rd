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

\usage{
convertCel(filename, outFilename, readMap=NULL, writeMap=NULL, version="4",
  newChipType=NULL, ..., .validate=FALSE, verbose=FALSE)
}

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
  \item{newChipType}{(Only for advanced users who fully understands
     the Affymetrix CEL file format!)
     An optional string for overriding the chip type (label)
     in the CEL file header.}
  \item{...}{Not used.}
  \item{.validate}{If \code{\link[base:logical]{TRUE}}, a consistency test between the generated
    and the original CEL is performed.}
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

\section{WARNING: Changing the chip type label}{
  The \code{newChipType} argument changes the label in the
  part of DAT header that specifies the chip type of the
  CEL file.  Note that it does not change anything else in
  the CEL file.  This type of relabelling is valid for updating
  the chip type \emph{label} of CEL files that where generated
  during, say, an "Early Access" period leading to a different
  chip type label than what more recent CEL files of the same
  physical chip type have.
}

\examples{
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Search for some available Calvin CEL files
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE, firstOnly=FALSE)
files <- grep("FusionSDK_Test3", files, value=TRUE)
files <- grep("Calvin", files, value=TRUE)
file <- files[1]


outFile <- file.path(tempdir(), gsub("[.]CEL$", ",XBA.CEL", basename(file)))
if (file.exists(outFile))
  file.remove(outFile)
convertCel(file, outFile, .validate=TRUE)


##############################################################
}                                                     # STOP #
##############################################################

}

\author{Henrik Bengtsson}

\seealso{
  \code{\link{createCel}}().
}



\keyword{file}
\keyword{IO}

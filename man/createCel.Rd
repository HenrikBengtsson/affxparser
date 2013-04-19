%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  createCel.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{createCel}
\alias{createCel}


 \title{Creates an empty CEL file}

 \usage{
createCel(filename, header, nsubgrids=0, overwrite=FALSE, ..., cdf=NULL, verbose=FALSE)
}

 \description{
   Creates an empty CEL file.
 }

 \arguments{
   \item{filename}{The filename of the CEL file to be created.}
   \item{header}{A \code{\link[base]{list}} structure describing the CEL header, similar
     to the structure returned by \code{\link{readCelHeader}}().  This header
     can be of any CEL header version.}
   \item{overwrite}{If \code{\link[base:logical]{FALSE}} and the file already exists, an exception
     is thrown, otherwise the file is created.}
   \item{nsubgrids}{The number of subgrids.}
   \item{...}{Not used.}
   \item{cdf}{(optional) The pathname of a CDF file for the CEL file
     to be created.  If given, the CEL header (argument \code{header})
     is validated against the CDF header, otherwise not.
     If \code{\link[base:logical]{TRUE}}, a CDF file is located automatically based using
     \code{findCdf(header$chiptype)}.
   }
   \item{verbose}{An \code{\link[base]{integer}} specifying how much verbose details are
     outputted.}
 }

 \value{
   Returns (invisibly) the pathname of the file created.
 }

 \details{
   Currently only binary (v4) CEL files are supported.
   The current version of the method does not make use of the Fusion SDK,
   but its own code to create the CEL file.
 }

 \section{Redundant fields in the CEL header}{
   There are a few redundant fields in the CEL header.  To make sure
   the CEL header is consistent, redundant fields are cleared and
   regenerated.  For instance, the field for the total number of cells
   is calculated from the number of cell rows and columns.
 }

 \examples{
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Search for first available ASCII CEL file
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE, firstOnly=FALSE)
files <- grep("ASCII", files, value=TRUE)
file <- files[1]


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Read the CEL header
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
hdr <- readCelHeader(file)

# Assert that we found an ASCII CEL file, but any will do
stopifnot(hdr$version == 3)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Create a CEL v4 file of the same chip type
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
outFile <- file.path(tempdir(), "zzz.CEL")
if (file.exists(outFile))
  file.remove(outFile)
createCel(outFile, hdr, overwrite=TRUE)
str(readCelHeader(outFile))

# Verify correctness by update and re-read a few cells
intensities <- as.double(1:100)
indices <- seq(along=intensities)
updateCel(outFile, indices=indices, intensities=intensities)
value <- readCel(outFile, indices=indices)$intensities
stopifnot(identical(intensities, value))


##############################################################
}                                                     # STOP #
##############################################################
}

 \author{Henrik Bengtsson}



\keyword{file}
\keyword{IO}

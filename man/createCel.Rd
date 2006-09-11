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

 \usage{createCel(filename, header, nsubgrids=0, overwrite=FALSE, ..., verbose=FALSE)}

 \description{
   Creates an empty CEL file.
 }

 \arguments{
   \item{filename}{The filename of the CEL file to be created.}
   \item{header}{A \code{\link[base]{list}} structure describing the CEL header, similar
     to the structure returned by \code{\link{readCelHeader}}().}
   \item{overwrite}{If \code{\link[base:logical]{FALSE}} and the file already exists, an exception
     is thrown, otherwise the file is created.}
   \item{nsubgrids}{The number of subgrids.}
   \item{...}{Not used.}
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
for (zzz in 0) {  # Only so that 'break' can be used

# Scan current directory for CEL files
outFile <- "zzz.CEL"
files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
files <- setdiff(files, outFile)
if (length(files) == 0)
  break

file <- files[1]


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Read the CEL header
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
hdr <- readCelHeader(file)
if (hdr$version != 4)
  break

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Create a CEL v4 file of the same chip type
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
createCel(outFile, hdr, overwrite=TRUE)
str(readCelHeader(outFile))

# Verify correctness by update and re-read a few cells
intensities <- as.double(1:100)
indices <- seq(along=intensities)
updateCel(outFile, indices=indices, intensities=intensities)
value <- readCel(outFile, indices=indices)$intensities
stopifnot(identical(intensities, value))

# Clean up
#rm(files, file, hdr, hdr2)
} # for (zzz in 0)
}

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}



\keyword{file}
\keyword{IO}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  updateCel.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{updateCel}
\alias{updateCel}


 \title{Updates a CEL file}

 \usage{updateCel(filename, indices=NULL, intensities=NULL, stdvs=NULL, pixels=NULL, ..., verbose=FALSE)}

 \description{
   Updates a CEL file.
 }

 \arguments{
   \item{filename}{The filename of the CEL file.}
   \item{indices}{A \code{\link[base]{numeric}} \code{\link[base]{vector}} of cell (probe) indices specifying
     which cells to updated.  If \code{\link[base]{NULL}}, all indices are considered.}
   \item{intensities}{A \code{\link[base]{numeric}} \code{\link[base]{vector}} of intensity values to be stored.
     Alternatively, it can also be a named \code{\link[base]{data.frame}} or \code{\link[base]{matrix}} (or \code{\link[base]{list}})
     where the named columns (elements) are the fields to be updated.}
   \item{stdvs}{A optional \code{\link[base]{numeric}} \code{\link[base]{vector}}.}
   \item{pixels}{A optional \code{\link[base]{numeric}} \code{\link[base]{vector}}.}
   \item{...}{Not used.}
   \item{verbose}{An \code{\link[base]{integer}} specifying how much verbose details are
     outputted.}
 }

 \value{
   Returns (invisibly) the pathname of the file updated.
 }

 \details{
   Currently only binary (v4) CEL files are supported.
   The current version of the method does not make use of the Fusion SDK,
   but its own code to navigate and update the CEL file.
 }

 \examples{

for (zzz in 0) {  # Only so that 'break' can be used

# Scan current directory for CEL files
files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
if (length(files) == 0)
  break;

# Create tempory copy of the CEL file
filename <- file.path(tempdir(), files[1])
if (!file.exists(filename))
  file.copy(from=files[1], to=filename)

fields <- c("intensities", "stdvs", "pixels")

# Cells to be updated
idxs <- 1:2;

# Get the original data
cel <- readCel(filename, indices=idxs, readStdvs=TRUE, readPixels=TRUE)
print(cel[fields])
cel0 <- cel

# Square-root the intensities
updateCel(filename, indices=idxs, intensities=sqrt(cel$intensities))
cel <- readCel(filename, indices=idxs, readStdvs=TRUE, readPixels=TRUE)
print(cel[fields])

# Update a few cell values by a data frame
data <- data.frame(
  intensities=cel0$intensities,
  stdvs=c(201.1, 3086.1)+0.5,
  pixels=c(9,9+1)
)
updateCel(filename, indices=idxs, data);

# Assert correctness of update
cel <- readCel(filename, indices=idxs, readStdvs=TRUE, readPixels=TRUE)
print(cel[fields])
for (ff in fields) {
  stopifnot(all.equal(cel[[ff]], data[[ff]], sqrt(.Machine$double.eps)^0.5))
}

# Make some spatial changes
rotate270 <- function(x, ...) {
  x <- t(x)
  nc <- ncol(x)
  if (nc < 2) return(x)
  x[,nc:1,drop=FALSE]
}
updateCel(filename, indices=1:6144, intensities=rep(c(0,32768,65535), length.out=6144))
cel <- readCelRectangle(filename, xrange=c(0,250), yrange=c(0,250))
z <- rotate270(cel$intensities)
sub <- paste("Chip type:", cel$header$chiptype)
image(z, col=gray.colors(256), axes=FALSE, main=filename, sub=sub)
text(x=0, y=1, labels="(0,0)", adj=c(0,-0.7), cex=0.8, xpd=TRUE)
text(x=1, y=0, labels="(250,250)", adj=c(1,1.2), cex=0.8, xpd=TRUE)



# Revert changes
updateCel(filename, indices=idxs, cel0)

# Clean up
rm(files, cel, cel0, idxs, data, ff, fields, rotate270)

} # for (zzz in 0)
}

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}



\keyword{file}
\keyword{IO}

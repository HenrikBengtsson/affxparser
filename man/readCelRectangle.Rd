%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCelRectangle.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCelRectangle}
\alias{readCelRectangle}


 \title{Reads a spatial subset of probe-level data from Affymetrix CEL files}

 \usage{readCelRectangle(filename, xrange=c(0, Inf), yrange=c(0, Inf), ..., asMatrix=TRUE)}

 \description{
   Reads a spatial subset of probe-level data from Affymetrix CEL files.
 }

 \arguments{
   \item{filename}{The pathname of the CEL file.}
   \item{xrange}{A \code{\link[base]{numeric}} \code{\link[base]{vector}} of length two giving the left
     and right coordinates of the cells to be returned.}
   \item{yrange}{A \code{\link[base]{numeric}} \code{\link[base]{vector}} of length two giving the top
     and bottom coordinates of the cells to be returned.}
   \item{...}{Additional arguments passed to \code{\link{readCel}}().}
   \item{asMatrix}{If \code{\link[base:logical]{TRUE}}, the CEL data fields are returned as
     matrices with element (1,1) corresponding to cell
     (xrange[1],yrange[1]).}
 }

 \value{
   A named \code{\link[base]{list}} CEL structure similar to what \code{\link{readCel}}().
   In addition, if \code{asMatrix} is \code{\link[base:logical]{TRUE}}, the CEL data fields
   are returned as matrices, otherwise not.
 }

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}

 \examples{

for (zzz in 0) {

rotate270 <- function(x, ...) {
  x <- t(x)
  nc <- ncol(x)
  if (nc < 2) return(x)
  x[,nc:1,drop=FALSE]
}

# Scan current directory for CEL files
files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
files <- files[!file.info(files)$isdir]
if (length(files) == 0)
  break

celFile <- files[1];

# Read CEL intensities in the upper left corner
cel <- readCelRectangle(celFile, xrange=c(0,250), yrange=c(0,250))
z <- rotate270(cel$intensities)
sub <- paste("Chip type:", cel$header$chiptype)
image(z, col=gray.colors(256), axes=FALSE, main=celFile, sub=sub)
text(x=0, y=1, labels="(0,0)", adj=c(0,-0.7), cex=0.8, xpd=TRUE)
text(x=1, y=0, labels="(250,250)", adj=c(1,1.2), cex=0.8, xpd=TRUE)

# Clean up
rm(rotate270, files, celFile, cel, z, sub)

} # for (zzz in 0)
}

 \seealso{
   The \code{\link{readCel}}() method is used internally.
 }



\keyword{file}
\keyword{IO}

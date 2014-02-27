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

\usage{
readCelRectangle(filename, xrange=c(0, Inf), yrange=c(0, Inf), ..., asMatrix=TRUE)
}

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

\author{Henrik Bengtsson}

\examples{
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

rotate270 <- function(x, ...) {
  x <- t(x)
  nc <- ncol(x)
  if (nc < 2) return(x)
  x[,nc:1,drop=FALSE]
}


# Search for some available CEL files
path <- system.file("rawData", package="AffymetrixDataTestFiles")
file <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE)


# Read CEL intensities in the upper left corner
cel <- readCelRectangle(file, xrange=c(0,250), yrange=c(0,250))
z <- rotate270(cel$intensities)
sub <- paste("Chip type:", cel$header$chiptype)
image(z, col=gray.colors(256), axes=FALSE, main=basename(file), sub=sub)
text(x=0, y=1, labels="(0,0)", adj=c(0,-0.7), cex=0.8, xpd=TRUE)
text(x=1, y=0, labels="(250,250)", adj=c(1,1.2), cex=0.8, xpd=TRUE)

# Clean up
rm(rotate270, files, file, cel, z, sub)


##############################################################
}                                                     # STOP #
##############################################################

}

\seealso{
  The \code{\link{readCel}}() method is used internally.
}



\keyword{file}
\keyword{IO}

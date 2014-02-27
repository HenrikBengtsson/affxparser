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

\usage{
updateCel(filename, indices=NULL, intensities=NULL, stdvs=NULL, pixels=NULL,
  writeMap=NULL, ..., verbose=0)
}

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
  \item{writeMap}{An optional write map.}
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
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Search for some available Calvin CEL files
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE, firstOnly=FALSE)
files <- grep("FusionSDK_HG-U133A", files, value=TRUE)
files <- grep("Calvin", files, value=TRUE)
file <- files[1]

# Convert to an XDA CEL file
filename <- file.path(tempdir(), basename(file))
if (file.exists(filename))
  file.remove(filename)
convertCel(file, filename)


fields <- c("intensities", "stdvs", "pixels")

# Cells to be updated
idxs <- 1:2

# Get CEL header
hdr <- readCelHeader(filename)

# Get the original data
cel <- readCel(filename, indices=idxs, readStdvs=TRUE, readPixels=TRUE)
print(cel[fields])
cel0 <- cel

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Square-root the intensities
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
updateCel(filename, indices=idxs, intensities=sqrt(cel$intensities))
cel <- readCel(filename, indices=idxs, readStdvs=TRUE, readPixels=TRUE)
print(cel[fields])


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Update a few cell values by a data frame
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data <- data.frame(
  intensities=cel0$intensities,
  stdvs=c(201.1, 3086.1)+0.5,
  pixels=c(9,9+1)
)
updateCel(filename, indices=idxs, data)

# Assert correctness of update
cel <- readCel(filename, indices=idxs, readStdvs=TRUE, readPixels=TRUE)
print(cel[fields])
for (ff in fields) {
  stopifnot(all.equal(cel[[ff]], data[[ff]], .Machine$double.eps^0.25))
}

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Update a region of the CEL file
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Load pre-defined data
side <- 306
pathname <- system.file("extras/easternEgg.gz", package="affxparser")
con <- gzfile(pathname, open="rb")
z <- readBin(con=con, what="integer", size=1, signed=FALSE, n=side^2)
close(con)
z <- matrix(z, nrow=side)
side <- min(hdr$cols - 2*22, side)
z <- as.double(z[1:side,1:side])
x <- matrix(22+0:(side-1), nrow=side, ncol=side, byrow=TRUE)
idxs <- as.vector((1 + x) + hdr$cols*t(x))
# Load current data in the same region
z0 <- readCel(filename, indices=idxs)$intensities
# Mix the two data sets
z <- (0.3*z^2 + 0.7*z0)
# Update the CEL file
updateCel(filename, indices=idxs, intensities=z)

# Make some spatial changes
rotate270 <- function(x, ...) {
  x <- t(x)
  nc <- ncol(x)
  if (nc < 2) return(x)
  x[,nc:1,drop=FALSE]
}

# Display a spatial image of the updated CEL file
cel <- readCelRectangle(filename, xrange=c(0,350), yrange=c(0,350))
z <- rotate270(cel$intensities)
sub <- paste("Chip type:", cel$header$chiptype)
image(z, col=gray.colors(256), axes=FALSE, main=basename(filename), sub=sub)
text(x=0, y=1, labels="(0,0)", adj=c(0,-0.7), cex=0.8, xpd=TRUE)
text(x=1, y=0, labels="(350,350)", adj=c(1,1.2), cex=0.8, xpd=TRUE)


# Clean up
file.remove(filename)
rm(files, cel, cel0, idxs, data, ff, fields, rotate270)


##############################################################
}                                                     # STOP #
##############################################################

}

\author{Henrik Bengtsson}



\keyword{file}
\keyword{IO}

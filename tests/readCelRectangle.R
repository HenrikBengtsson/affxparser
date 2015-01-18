if (require("AffymetrixDataTestFiles")) {
  library("affxparser")

  rotate270 <- function(x, ...) {
    x <- t(x)
    nc <- ncol(x)
    if (nc < 2) return(x)
    x[,nc:1,drop=FALSE]
  }

  # Search for some available CEL files
  pathR <- system.file(package="AffymetrixDataTestFiles")
  pathD <- file.path(pathR, "rawData", "FusionSDK_HG-Focus", "HG-Focus")
  cel <- file.path(pathD, "2.Calvin", "HG-Focus-1-121502.CEL")

  # Read CEL intensities in the upper left corner
  range <- c(0,250)
  data <- readCelRectangle(cel, xrange=range, yrange=range)

  # Displaying image
  z <- rotate270(data$intensities)
  sub <- sprintf("Chip type: %s", data$header$chiptype)
  image(z, col=gray.colors(256), axes=FALSE, main=basename(cel), sub=sub)
  text(x=0, y=1, labels="(0,0)", adj=c(0,-0.7), cex=0.8, xpd=TRUE)
  text(x=1, y=0, labels="(250,250)", adj=c(1,1.2), cex=0.8, xpd=TRUE)

  # Read 1x1 rectangle
  range <- c(0,0)
  data <- readCelRectangle(cel, xrange=range, yrange=range)
  print(data$intensities)
  stopifnot(all(dim(data$intensities) == c(1,1)))
}



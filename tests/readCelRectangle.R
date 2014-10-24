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
  data <- readCelRectangle(cel, xrange=c(0,250), yrange=c(0,250))
  z <- rotate270(data$intensities)
  sub <- sprintf("Chip type: %s", data$header$chiptype)
  image(z, col=gray.colors(256), axes=FALSE, main=basename(cel), sub=sub)
  text(x=0, y=1, labels="(0,0)", adj=c(0,-0.7), cex=0.8, xpd=TRUE)
  text(x=1, y=0, labels="(250,250)", adj=c(1,1.2), cex=0.8, xpd=TRUE)
}



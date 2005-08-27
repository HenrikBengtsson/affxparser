##  affxparser.R : A utility for parsing Affymetrix files in R.
##
##  Copyright (C) 2004-2005 James Bullard, Kasper Daniel Hansen
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU Lesser General Public License as published by
##  the Free Software Foundation; either version 2.1 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU Lesser General Public License for more details.
##
##  You should have received a copy of the GNU Lesser General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
##  02111-1307  USA

##
## Functions for parsing CEL files
##

read.cel.complete <- function(fname, indices = NULL, read.intensity = TRUE,
                              read.std = TRUE, read.npixels = TRUE,
                              read.xy = FALSE, read.outliers = TRUE,
                              read.masked = TRUE, verbose = FALSE){


  cel.file <- .get.cel.file(as.character(fname),
                            readIntensities = as.integer(read.intensity),
                            readStdv = as.integer(read.std),
                            readPixels = as.integer(read.npixels),
                            readX = as.integer(read.xy),
                            readY = as.integer(read.xy),
                            readOutliers = as.integer(read.outliers),
                            readHeader = TRUE,
                            readMasked = as.integer(read.masked),
                            indices = as.integer(indices),
                            verbose = as.integer(verbose))
  return(cel.file)
}

read.cel.header <- function(fname){
  fname <- file.path(dirname(fname), basename(fname))
  
  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))

  cel.file.header <- .Call("R_affx_get_cel_file_header", as.character(fname))

  return(cel.file.header)
}


## XXX: right now there is a problem reading some of the header
## information when you dont read the actual data. I agree that
## we should have these sanity checks, but i am not so convinced

read.cel.intensity <- function(fnames, indices = NULL, verbose = FALSE){
    fnames <- sapply(fnames, path.expand)
    ## existence of the fnames gets check in the read.cel.header call
    if(verbose)
        cat("Entering read.cel.intensity\n ... reading headers\n")
    all.headers <- lapply(as.list(fnames), read.cel.header)

    ## XXX: need to fix the reading of the header. 
    #if(length(unique(sapply(all.headers, function(x) x$chipName))) != 1)
    #    stop("the associated chipnames are not the same")
    
    if(!all(diff(sapply(all.headers, function(x) x$cols))) |
       !all(diff(sapply(all.headers, function(x) x$rows))))
#        stop("dimensions of the files do not match")
    if(!is.null(indices))
        intensities.nrow <- length(indices)
    else
        intensities.nrow <-
          all.headers[[1]]$rows * all.headers[[1]]$rows  # numberOfFeatures
    if(verbose)
        cat(" ...allocating memory for intensity matrix\n") 
    intensities <- matrix(NA, ncol = length(fnames), nrow = intensities.nrow)
    for(i in 1:length(fnames)){
        if(verbose)
            cat(" ... reading", fnames[i], "\n")
        intensities[, i] <- read.cel.complete(fname = fnames[i],
                                              indices = indices,
                                              read.intensity = TRUE,
                                              read.std = FALSE,
                                              read.npixels = FALSE,
                                              read.xy = FALSE,
                                              read.outliers = FALSE,
                                              read.masked = FALSE, 
                                              verbose = verbose)$intensities
    }
    return(intensities)
}


.get.cel.file <- function(fname, readHeaders = FALSE, readIntensities =
                          TRUE, readX = FALSE, readY = FALSE, readPixels
                          = FALSE, readStdv = FALSE, readOutliers =
                          FALSE, readMasked = FALSE, indices = NULL,
                          verbose = 0) {
  fname <- file.path(dirname(fname), basename(fname))
  
  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))

  cel.file <- .Call("R_affx_get_cel_file", as.character(fname),
                    as.integer(readHeaders),
                    as.integer(readIntensities), as.integer(readX),
                    as.integer(readY), as.integer(readPixels),
                    as.integer(readStdv), as.integer(readOutliers),
                    as.integer(readMasked), as.integer(indices),
                    as.integer(verbose))
  
  return(cel.file)
}

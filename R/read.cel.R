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

read.cel.header <- function(fname){
    fname <- file.path(dirname(fname), basename(fname))
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))
    return(.Call("R_affx_get_cel_file_header", as.character(fname)))
}

read.cel.complete <- function(fname, indices = NULL,
                              read.intensities = TRUE,
                              read.stdvs = TRUE, read.pixels = TRUE,
                              read.xy = FALSE, read.outliers = TRUE,
                              read.masked = TRUE, verbose = 0){
    ## Need sanity check on indices argument and other.
    fname <- file.path(dirname(fname), basename(fname))
    if(length(fname) != 1)
        stop("This function only reads a single cel file.")
    if (!file.exists(fname))
        stop(paste("file:", fname, "does not exist."))
    cel.file <- .Call("R_affx_get_cel_file", fname,
                      readHeader = TRUE,
                      readIntensities = read.intensities,
                      readX = read.xy,
                      readY = read.xy,
                      readPixels = read.pixels,
                      readStdvs = read.stdvs,
                      readOutliers = read.outliers,
                      readMasked = read.masked,
                      indicesToRead = indices,
                      verbose = verbose)
    return(cel.file)
}

read.cel.intensities <- function(fnames, indices = NULL, verbose = 0){
     fnames <- file.path(dirname(fnames), basename(fnames))
     if(!all(file.exists(fnames)))
         stop(paste(c(" ", paste("file:",fnames[!file.exists(fnames)],
                          "does not exist.")), collapse = "\n"))
     if(verbose > 0)
         cat("Entering read.cel.intensity\n ... reading headers\n")
     all.headers <- lapply(as.list(fnames), read.cel.header)
     nrows <- unique(sapply(all.headers, function(x) x$rows))
     ncols <- unique(sapply(all.headers, function(x) x$cols))
     chiptype <- unique(sapply(all.headers, function(x) x$chiptype))
     nfiles <- length(fnames)
     if(length(nrows) != 1 || length(ncols) != 1)
         stop("The CEL files dimension do not match.")
     if(length(chiptype) != 1)
         warning("The CEL files do not have the same chiptype.")
     if(verbose > 0)
         cat(" ...allocating memory for intensity matrix\n")
     if(is.null(indices))
         intensities <- matrix(NA, nrow = nrows * ncols, ncol = nfiles)
     else
         intensities <- matrix(NA, nrow = length(indices), ncol = nfiles)
     colnames(intensities) <- fnames
     for(i in 1:nfiles){
         if(verbose > 0)
            cat(" ... reading", fnames[i], "\n")
         intensities[, i] <- read.cel.complete(fname = fnames[i],
                                               indices = indices,
                                               read.intensities = TRUE,
                                               read.stdvs = FALSE,
                                               read.pixels = FALSE,
                                               read.xy = FALSE,
                                               read.outliers = FALSE,
                                               read.masked = FALSE, 
                                 verbose = (verbose - 1))$intensities
     }
     return(intensities)
 }

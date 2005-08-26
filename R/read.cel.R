##
## Functions for parsing CEL files
##

read.cel.complete <- function(fname, indices = NULL, read.intensity = TRUE,
                         read.std = TRUE, read.npixels = TRUE,
                         read.outliers = TRUE, read.masked = TRUE, 
                         debug = FALSE){
    fname <-fname(path.expand(fname))
    if(!file.exists(fname))
        stop(paste(fname, "does not exists."))
    print("Place holder function")
}

read.cel.header <- function(fname){
    read.cel.complete(fname = fname, indices = NULL, read.intensity = FALSE,
                      read.std = FALSE, read.npixels = FALSE,
                      read.outliers = FALSE, read.masked = FALSE,
                      debug = FALSE)$header
}

read.cel.intensity <- function(fnames, indices = NULL, debug = FALSE){
    fnames <- sapply(fnames, path.expand)
    ## existence of the fnames gets check in the read.cel.header call
    if(debug)
        cat("Debugging read.cel.intensity\n ... reading headers\n")
    all.headers <- lapply(as.list(fnames), read.cel.header)
    if(length(unique(sapply(all.headers, function(x) x$chipName))) != 1)
        stop("the associated chipnames are not the same")
    if(!all(diff(sapply(all.headers, function(x) x$numberOfFeatures))) |
       !all(diff(sapply(all.headers, function(x) x$numberOfRows))) |
       !all(diff(sapply(all.headers, function(x) x$numberOfColumns))))
        stop("dimensions of the files do not match")
    if(!is.null(indices))
        intensities.nrow <- length(indices)
    else
        intensities.nrow <- all.headers[[1]]$numberOfFeatures
    if(debug)
        cat(" ...allocating memory for intensity matrix\n") 
    intensities <- matrix(NA, ncol = length(fnames), nrow = intensities.nrow)
    for(i in length(fnames)){
        if(debug)
            cat(" ... reading", fnames[i], "\n")
        intensities[, i] <- read.cel.complete(fname = fnames[i],
                                              indices = indices,
                                              read.intensity = TRUE,
                                              read.std = FALSE,
                                              read.npixels = FALSE,
                                              read.outliers = FALSE,
                                              read.masked = FALSE, 
                                              debug = debug)$intensity
    }
    return(intensities)
}

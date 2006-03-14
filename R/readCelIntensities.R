readCelIntensities <- function(fnames, indices = NULL, verbose = 0){
     fnames <- file.path(dirname(fnames), basename(fnames))
     if(!all(file.exists(fnames)))
         stop(paste(c(" ", paste("file:",fnames[!file.exists(fnames)],
                          "does not exist.")), collapse = "\n"))
     if(verbose > 0)
         cat("Entering read.cel.intensity\n ... reading headers\n")
     all.headers <- lapply(as.list(fnames), readCelHeader)
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
         intensities[, i] <- readCel(fname = fnames[i],
                                     indices = indices,
                                     readIntensities = TRUE,
                                     readHeader = FALSE,
                                     readStdvs = FALSE,
                                     readPixels = FALSE,
                                     readXY = FALSE,
                                     readOutliers = FALSE,
                                     readMasked = FALSE, 
                                     verbose = (verbose - 1))$intensities
     }
     return(intensities)
 }

read.affybatch2 <- function(..., filenames = character(0),
                            phenoData = new("phenoData"), 
                            description = NULL, notes = "",
                            compress = getOption("BioC")$affy$compress.cel, 
                            rm.mask = FALSE, rm.outliers = FALSE,
                            rm.extra = FALSE, verbose = FALSE) 
{
    auxnames <- as.list(substitute(list(...)))[-1]
    filenames <- .Primitive("c")(filenames, auxnames)
    n <- length(filenames)
    if (n == 0) 
        stop("No file name given !")
    pdata <- pData(phenoData)
    if (dim(pdata)[1] != n) {
        warning("Incompatible phenoData object. Created a new one.\n")
        samplenames <- sub("^/?([^/]*/)*", "", unlist(filenames), 
            extended = TRUE)
        pdata <- data.frame(sample = 1:n, row.names = samplenames)
        phenoData <- new("phenoData", pData = pdata,
                         varLabels = list(sample = "arbitrary numbering")) 
    }
    else samplenames <- rownames(pdata)
    if (is.null(description)) {
        description <- new("MIAME")
        description@preprocessing$filenames <- filenames
        description@preprocessing$affyversion <-
            library(help = affy)$info[[2]][[2]][2]
    }
    one.header <- read.cel.header(filenames[1])
    return(new("AffyBatch", exprs = read.cel.intensity(fnames = filenames,
                            indices = NULL, debug = verbose),
               cdfName = one.header$chipName,
               phenoData = phenoData, nrow = one.header$numberOfRows,
               ncol = one.header$numberOfCols,
               annotation = cleancdfname(one.header$chipName, addcdf = FALSE), 
               description = description, notes = notes))
}

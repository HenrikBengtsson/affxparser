read.affybatch2 <- function(..., filenames = character(0),
                            phenoData = new("phenoData"), 
                            description = NULL, notes = "",
                            compress = getOption("BioC")$affy$compress.cel, 
                            rm.mask = FALSE, rm.outliers = FALSE,
                            rm.extra = FALSE, verbose = FALSE) 
{
    require(Biobase) || stop("Package not found: Biobase")
    require(affy) || stop("Package not found: affy")

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
    filenames <- unlist(filenames)
    one.header <- read.cel.header(filenames[1])
    return(new("AffyBatch", exprs = read.cel.intensities(fnames =
                            unlist(filenames), indices = NULL,
                            verbose = as.integer(verbose)),
               cdfName = one.header$chiptype,
               phenoData = phenoData, nrow = one.header$rows,
               ncol = one.header$cols,
               annotation = cleancdfname(one.header$chiptype, addcdf = FALSE), 
               description = description, notes = notes))
}

############################################################################
# HISTORY:
# 2006-01-10
# o Added require(Biobase) after removing 'Depends: Biobase' in DESCRIPTION.
# o Extracted to its own source file. /HB
############################################################################  

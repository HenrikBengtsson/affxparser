library(affxparser)
library(affy)
sessionInfo()
place <- "/Users/kdh/work/bioc/data/hgu133"
files <- c("HG-U133A-1-121502.CEL",
           "HG-U133A-2-121502.CEL",
           "HG-U133A-Tissue1-121502.CEL",
           "HG-U133A-Tissue2-121502.CEL")
filesWplace <- paste(place, files, sep = "/")
system.time(
            tmp.affy <- read.affybatch(filenames = filesWplace)
            )
system.time(
            tmp.affxparser <- read.affybatch2(filenames = filesWplace)
            )

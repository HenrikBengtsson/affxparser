celnames <- c("Test3-Tissue1-121502.cel",
              "Test3-Tissue1-121502.CEL",
              "Test3-Tissue1-121502.calvin.CEL")
cdfnames <- c("Test3.CDF", "Test3.expression.CDF")

if(!all(file.exists(c(celnames, cdfnames))))
    stop("Missing files...")
headers <- lapply(as.list(celnames), read.cel.header)

affxparserInfo <- function(){
    writeLines(readLines(system.file("README", package = "affxparser"),
                         n = -1))
}

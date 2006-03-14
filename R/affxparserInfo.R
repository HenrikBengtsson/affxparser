affxparserInfo <- function(fusion.license = FALSE,
                           file.changes = FALSE){
    writeLines(readLines(system.file("info/README",
                                     package = "affxparser"),
                         n = -1))
    if(file.changes)
        writeLines(readLines(system.file("info/CHANGES",
                                         package = "affxparser"),
                             n = -1))
    if(fusion.license)
        browseURL(system.file("info/fusion_sdk_license.html",
                                     package = "affxparser"))
}

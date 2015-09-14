writeTpmap <- function(filename, bpmaplist, verbose = 0){
    writeSequence <- function(seq){
        if(length(setdiff(c("seqInfo", "pmx", "pmy", "probeseq", "startpos", "strand"),
                          names(seq))) != 0 ||
           length(setdiff(c("groupname", "version", "name"),
                          names(seq$seqInfo))) != 0) {
            cat(" ... skipping a sequence due to missing slots\n")
            return(NULL)
        }
        seqInfo <- seq$seqInfo
        if(length(setdiff(c("groupname", "version", "name"), names(seqInfo))) != 0) {
            stop("Need a seqInfo component with 'groupname', 'version', 'name' sub-components")
        }
        writeLines(paste("#seq_group_name", seqInfo$groupname), con = out, sep = "\n")
        writeLines(paste("#version", seqInfo$version), con = out, sep = "\n")
        if(!is.null(seqInfo$parameters)) {
            for(tag in names(seqInfo$parameters))
                writeLines(paste("#", tag, " ", seqInfo$parameters[tag], sep = ""),
                           con = out, sep = "\n")
        }
        hits <- t(do.call(cbind, c(seq[c("probeseq", "strand")],
                                   list(groupname = rep(seqInfo$name, length(seq$pmx))),
                                   seq[c("startpos", "pmx", "pmy", "mmx", "mmy", "matchscore")])))
        write(hits, file = out, ncolumns = nrow(hits), append = TRUE)
        return(NULL)
    } # writeSequence()

    if (file.exists(filename)) {
      stop("Could not write TPMAP file. File already exists: ", filename)
    }

    out <- file(filename, open = "w")
    on.exit(close(out))

    for(i in seq_along(bpmaplist)) {
        if(verbose)
            cat(paste("Writing sequence", names(bpmaplist)[i], "\n"))
        writeSequence(bpmaplist[[i]])
    }

    invisible(NULL)
}

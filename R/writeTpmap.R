writeTpmap <- function(filename, bpmaplist, verbose = 0){
    out <- file(filename, open = "w")
    writeSequence <- function(seq){
        if(length(setdiff(c("seqInfo", "pmx", "pmy", "probeseq", "startpos", "strand"),
                          names(seq))) != 0 ||
           length(setdiff(c("groupname", "version", "name"),
                          names(seq$seqInfo))) != 0) {
            cat(" ... skipping a sequence due to missing slots\n")
            return(NULL)
        }
        seqInfo <- seq$seqInfo
        if(length(setdiff(names(seqInfo))) != 0)
            
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
    }
    for(i in 1:length(bpmaplist)) {
        if(verbose)
            cat(paste("Writing sequence", names(bpmaplist)[i], "\n"))
        writeSequence(bpmaplist[[i]])
    }
    close(out)
    invisible(NULL)
}

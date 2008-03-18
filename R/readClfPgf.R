readClf <- function(file, readBody=TRUE) {
    ## FIXME: this is an exception in more recent TsvFile.cpp
    if (!file.exists(file))
        stop("clf file '", file, "' does not exist")
    env <- new.env(parent=emptyenv())
    .Call("R_affx_get_clf_file", file, readBody, env,
          PACKAGE="affxparser")
}

readPgf <- function(file, readBody=TRUE) {
    ## FIXME: this is an exception in more recent TsvFile.cpp
    if (!file.exists(file))
        stop("pgf file '", file, "' does not exist")
    env <- new.env(parent=emptyenv())
    .Call("R_affx_get_pgf_file", file, readBody, env,
          PACKAGE="affxparser")
}

.pgfProbeIndexFromProbesetIndex <- function(pgf, probesetIdx) {
    ## probeset -> atom
    astarts <- pgf[["probesetStartAtom"]][probesetIdx]
    aends <- pgf[["probesetStartAtom"]][probesetIdx+1]
    aends[is.na(aends)] <- length(pgf[["atomId"]]) + 1
    ## atom -> probe
    pstarts <- pgf[["atomStartProbe"]][astarts]
    pends <- pgf[["atomStartProbe"]][aends] - 1
    pends[is.na(pends)] <- length(pgf[["probeId"]])
    mapply(":", pstarts, pends, SIMPLIFY=FALSE)
}

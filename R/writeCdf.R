initializeCdf <- function(con, nrows = 5, ncols = 5,
                          nunits = 1, nqcunits = 0,
                          refseq = "") {
    if(length(refseq) != 1)
        stop("refseq argument wrong")
    lrefseq <- nchar(refseq)
    ## Magic number and version number
    writeBin(object = as.integer(c(67, 1)),
             con = con, size = 4, endian = "little")
    ## Nrows, Ncols
    writeBin(object = as.integer(c(nrows, ncols)),
             con = con, size = 2, endian = "little")
    ## NumberUnits, NumberQCUnits
    writeBin(object = as.integer(c(nunits, nqcunits)),
             con = con, size = 4, endian = "little")
    ## Length of refseqsequence
    writeBin(object = as.integer(lrefseq),
             con = con, size = 4, endian = "little")
    ## Refseqsequece
    if(lrefseq > 0)
        writeChar(as.character(refseq),
                  con = con, eos = NULL)
    ## Unit names
    writeChar(as.character(rep(" ", nunits)),
              nchars = rep(64, nunits), con = con, eos = NULL)
    ## QC units file positions
    writeBin(as.integer(rep(1, nqcunits)),
             con = con, size = 4, endian = "little")
    ## Units file positions
    writeBin(as.integer(rep(2, nunits)),
             con = con, size = 4, endian = "little")
    ## positions return argument
    offset <- 24 + lrefseq
    positions <- list(unitName = offset,
                      qcunitFP = offset + 64 * nunits,
                      unitFP = offset + 64 * nunits)
    return(positions)
}

writeUnit <- function(unit, unitname, positions, con) {
    ## 1. Write the name in the start of the CDF file
    seek(con = con, where = positions$unitName,
         origin = "start")
    writeChar(as.character(unitname), nchars = 64,
              con = con, eos = NULL)
    ## 2. Jump to the end to start writing the unit
    seek(con = con, where = 1,
         origin = "end", rw = "write")
    endPosition <- seek(con = con, where = 1,
                        origin = "end", rw = "write")
    ## 3. Write the unit
    unittype <- switch(unit$unittype,
                       expression = 1,
                       genotyping = 2,
                       tag = 3,
                       resequencing = 4,
                       unknown = 5)
    unitdirection <- switch(unit$unitdirection,
                            nodirection = 0,
                            sense = 1,
                            antisense = 2,
                            unknown = 3)
    unitInfo <- as.integer(c(unittype, unitdirection,
                             unit$natoms, length(unit$blocks),
                             unit$ncells, unit$unitnumber,
                             unit$ncellsperatom))
    
    writeBin(unitInfo[1],
             con = con, size = 2, endian = "little")
    writeBin(unitInfo[2],
             con = con, size = 1, endian = "little")
    writeBin(unitInfo[3:6],
             con = con, size = 4, endian = "little")
    writeBin(unitInfo[7],
             con = con, size = 1, endian = "little")
    ## Writing each block in turn
    for(iblock in seq(along.with = unit$blocks)) {
        block <- unit$blocks[[iblock]]
        blockdirection <- switch(block$blockdirection,
                                 nodirection = 0,
                                 sense = 1,
                                 antisense = 2,
                                 unknown = 3)
        blockInfo <- as.integer(c(block$natoms, length(block$x),
                                  block$ncellsperatom,
                                  blockdirection, min(block$atoms, 0)))
        writeBin(blockInfo[1:2],
                 con = con, size = 4, endian = "little")
        writeBin(blockInfo[3:4],
                 con = con, size = 1, endian = "little")
        writeBin(blockInfo[5:6],
                 con = con, size = 4, endian = "little")
        writeChar(as.character(names(unit$blocks)[iblock]),
                  con = con, nchars = 64, eos = NULL) 
        ## Writing each cell in turn
        cells <- matrix(as.integer(c(block$atom, block$x,
                                     block$y, block$indexpos)),
                        ncol = 4)
        for(icell in seq(along.with = block$x)) {
            writeBin(cells[icell, 1],
                     con = con, size = 4, endian = "little")
            writeBin(cells[icell, 2:3],
                     con = con, size = 2, endian = "little")
            writeBin(cells[icell, 4],
                     con = con, size = 4, endian = "little")
            writeChar(as.character(c(block$pbase[icell],
                                     block$tbase[icell])),
                      con = con, nchars = c(1,1), eos = NULL)
        }
    }
    ## 4. Go back to the start of the file and write
    ##    where the unit starts
    seek(con = con, where = positions$unitFP,
         origin = "start", rw = "write")
    writeBin(as.integer(endPosition),
             con = con, size = 4, endian = "little")
    ## 5. Update the positions and return
    positions$unitName <- positions$unitName + 64
    positions$unitFP <- positions$unitFP + 4
    return(positions)
}


writeCdf <- function(fname, cdf, verbose = 0) {
    if(verbose)
        cat("Trying to create CDF file\n  ", fname, "\n")
    cdfcon <- file(fname, open = "wb")
    positions <- initializeCdf(con = cdfcon, nunits = 2)
    for(i in seq(along.with = cdf)){
        if(verbose)
            cat(names(cdf)[i], "\n")
        positions <- writeUnit(unit = cdf[[i]],
                               unitname = names(cdf)[i],
                               positions = positions,
                               con = cdfcon)
    }
    close(cdfcon)
    if(verbose)
        cat("CDF file created.\n")
    return(invisible(NULL))
}


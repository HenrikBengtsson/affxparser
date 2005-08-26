read.cdf.complete <- function(fname){
    fname <- path.expand(fname)
    if(!file.exists(fname))
        stop(paste(fname, "does not exist."))
    print("Placeholder function")
}

read.cdf.header <- function(fname){
    read.cdf.complete(fname = fname)
    print("Placeholder function")
}


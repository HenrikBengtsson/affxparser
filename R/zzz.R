.onUnload <- function (libpath) {
  ## covr: skip=1
  library.dynam.unload("affxparser", libpath)
}


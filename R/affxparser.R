
##  affxparser.R : A utility for parsing Affymetrix files in R.
##
##  Copyright (C) 2004-2005 James Bullard
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU Lesser General Public License as published by
##  the Free Software Foundation; either version 2.1 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU Lesser General Public License for more details.
##
##  You should have received a copy of the GNU Lesser General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

##
## this is the public method which creates a new
## environment from the cdf file.
##
get.cdf.env <- function(fname, complementary.logic = FALSE) {
  pmmm <- .parse.cdffile(fname, complementary.logic)
  e <- new.env()
  multiassign(names(pmmm), value = pmmm, e)
  e
}

##
## this method returns a list, the public method
## returns the environment.
##
.parse.cdffile <- function(fname, complementary.logic = FALSE) {
  fname <- file.path(dirname(fname), basename(fname))

  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))

  pmmm <- .Call("R_affx_get_pmmm_list", as.character(fname), as.integer(complementary.logic))

  if (is.null(pmmm) || length(pmmm) == 0)
    stop(paste("Error parsing:", fname))
  
  lapply(pmmm, t)
}

get.cel.file <- function(fname) {
  .get.cel.file(fname)
}

##
## this method returns a list of the *entire* cel file.
## the public method is more discerning. 
##
.get.cel.file <- function(fname) {
  fname <- file.path(dirname(fname), basename(fname))

  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))

  cel.file <- .Call("R_affx_get_cel_file", as.character(fname))

  colnames(cel.file$intensities) <- c("x","y","intensity","stdv","pixels")
  return(cel.file)
}

##
## this method constructs the affybatch object using the
## the parsing utilities in this library.
##
const.affybatch <- function(cel.files, ...){
  exprMatrix <- sapply(cel.files, function(x) x$intensities[,3])
  nrows <- cel.files[[1]]$header$rows
  ncols <- cel.files[[1]]$header$cols
  cdfname <- cel.files[[1]]$header$ChipType
  pheno <- new("phenoData", pData = data.frame(chip = seq(length = length(cel.files))),
               varLabels = list(chip = "Chip identifier"))

  abatch <- new("AffyBatch", exprs = exprMatrix, ncol = ncols,
             nrow = nrows, phenoData = pheno, cdfName = cdfname)

  return(abatch)
}

##
## what follows are some utility methods for testing
##
## FIXME: Remove on release of package. 

##
## compare the results of our cdfenv to that
## of make.cdf.env
##
.compare.parses <- function(fname) {
  cdf.env <- make.cdf.env(fname)
  print(paste(length(ls(cdf.env)), "probeset names from make.cdf.env"))
  pmmm <- .parse.cdffile(fname)
  print(paste(length(pmmm), "probeset names from get.cdf.env"))

  for (name in ls(cdf.env)) {
    if (any((pmmm[[name]] == get(name, cdf.env)) == FALSE)) {
      print(paste("Disagreement in", name))
      print(pmmm[name])
      print(get(name, cdf.env))
    }
  }
}

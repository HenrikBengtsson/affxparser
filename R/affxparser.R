
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


#############################################################################
##
## CEL Files
##
#############################################################################


getCelFile <- function(fname, readHeaders = FALSE, readIntensities =
                       TRUE, readX = FALSE, readY = FALSE, readPixels
                       = FALSE, readStdv = FALSE, readOutliers =
                       FALSE, readMasked = FALSE, indices = NULL) {
  fname <- file.path(dirname(fname), basename(fname))
  
  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))

  cel.file <- .Call("R_affx_get_cel_file", as.character(fname),
                    as.integer(readHeaders),
                    as.integer(readIntensities), as.integer(readX),
                    as.integer(readY), as.integer(readPixels),
                    as.integer(readStdv), as.integer(readOutliers),
                    as.integer(readMasked), as.integer(indices))
  return(cel.file)
}

##############################################################################
##
## CDF Files
##
##############################################################################

getCdfEnv <- function(fname, complementary.logic = FALSE) {
  pmmm <- .parse.cdffile(fname, complementary.logic)
  e <- new.env()
  multiassign(names(pmmm), value = pmmm, e)
  e
}

.getCdfEnv <- function(fname, complementary.logic = FALSE) {
  fname <- file.path(dirname(fname), basename(fname))

  if (!file.exists(fname))
    stop(paste("file:", fname, "does not exist."))

  pmmm <- .Call("R_affx_get_pmmm_list", as.character(fname), as.integer(complementary.logic))

  if (is.null(pmmm) || length(pmmm) == 0)
    stop(paste("Error parsing:", fname))
  
  lapply(pmmm, t)
}

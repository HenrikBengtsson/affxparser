%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdfDataFrame.R
% 
% on Tue Apr 15 12:40:44 2008.
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCdfDataFrame}
\alias{readCdfDataFrame}


 \title{Reads units (probesets) from an Affymetrix CDF file}

 \usage{readCdfDataFrame(filename, units=NULL, groups=NULL, cells=NULL, fields=NULL, drop=TRUE, verbose=0)}

 \description{
  Reads units (probesets) from an Affymetrix CDF file. Gets all or a subset of units (probesets).
 }

 \arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices
    specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
 }

 \value{
   An NxK \code{\link[base]{data.frame}}.
 }

 \author{
 }

 \examples{
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Find any CDF file
cdfFile <- findCdf()

units <- 101:120
fields <- c("unit", "unitName", "group", "groupName", "cell")
df <- readCdfDataFrame(cdfFile, units=units, fields=fields)
stopifnot(identical(sort(unique(df$unit)), units))

fields <- c("unit", "unitName", "unitType")
fields <- c(fields, "group", "groupName")
fields <- c(fields, "x", "y", "cell", "pbase", "tbase")
df <- readCdfDataFrame(cdfFile, units=units, fields=fields)
stopifnot(identical(sort(unique(df$unit)), units))


##############################################################
}                                                     # STOP #
##############################################################

}

 \seealso{
   For retrieving the CDF as a \code{\link[base]{list}} structure, see \code{\link{readCdfUnits}}().
 }

 \references{
   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
       June 14, 2005.
       \url{http://www.affymetrix.com/support/developer/}
 }




\keyword{file}
\keyword{IO}
\keyword{internal}

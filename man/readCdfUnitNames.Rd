
\name{readCdfUnitNames}
\alias{readCdfUnitNames}
\title{Reads unit (probeset) names from an Affymetrix CDF file}
\usage{readCdfUnitNames(filename, units=NULL, verbose=0)}

\description{
  Gets the names of all or a subset of units (probesets) in an
  Affymetrix CDF file.
  This can be used to get a map between unit names an the internal unit
  indices used by the CDF file.
}

\arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}}
    of unit indices specifying which units to be read.
    If \code{\link[base]{NULL}}, all units are read.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose
    level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
 }

 \value{
   A \code{\link[base]{character}} \code{\link[base]{vector}} of unit names.
 }
 
 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}
 
 \seealso{
   \code{\link{readCdfUnits}}().
 }
 
 \examples{\dontrun{See help(readCdfUnits) for an example}}
 


\keyword{file}
\keyword{IO}

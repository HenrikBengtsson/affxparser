\name{readCdfIsPm}
\alias{readCdfIsPm}

 \title{Checks if cells each group of each unit are mis-match probes}

 \usage{readCdfIsPm(filename, units=NULL, verbose=0)}

 \description{
  Checks if cells each group of each unit are mis-match probes.
 }

 \arguments{
   \item{filename}{The filename of the CDF file.}
   \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of (zero-based) unit indices
     specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
   \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
     file is parsed quietly.  The higher numbers, the more details.}
 }

 \value{
   A named \code{\link[base]{list}} of named \code{\link[base]{logical}} vectors.  The name of the list elements
   are unit names and the names of the logical vector are group names.
 }

 \author{Henrik Bengtsson, \email{hb@maths.lth.se}.}



\keyword{file}
\keyword{IO}

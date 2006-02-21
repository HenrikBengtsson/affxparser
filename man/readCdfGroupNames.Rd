 \name{readCdfGroupNames}
\alias{readCdfGroupNames}


 \title{Reads group names of a set units (probesets) in an Affymetrix CDF file}

 \usage{readCdfGroupNames(filename, units=NULL, verbose=0)}

 \description{
   Reads group names of a set units (probesets) in an Affymetrix CDF file.

   This is for instance useful for SNP arrays where the nucleotides used
   for the A and B alleles are the same as the group names.
 }

 \arguments{
   \item{filename}{The filename of the CDF file.}
   \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of (zero-based) unit indices
     specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
   \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
     file is parsed quietly.  The higher numbers, the more details.}
 }

 \value{
   A named \code{\link[base]{list}} structure where the names of the elements are the names
   of the units read.  Each element is a \code{\link[base]{character}} \code{\link[base]{vector}} with group
   names for the corrsponding unit.
 }

 \author{Henrik Bengtsson, \email{hb@maths.lth.se}.}

 \seealso{
   \code{\link{readCdfUnits}}().
 }



\keyword{file}
\keyword{IO}
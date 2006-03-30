
\name{readCdfUnits}
\alias{readCdfUnits}

\title{Reads units (probesets) from an Affymetrix CDF file}

 \usage{readCdfUnits(filename, units=NULL, readXY=TRUE, readBases=TRUE, readExpos=TRUE, readType=TRUE, readDirection=TRUE, stratifyBy=c("nothing", "pmmm", "pm", "mm"), readIndices=FALSE, verbose=0)}

\description{
  Gets the all or a subset of units (probesets) in an Affymetrix CDF file.
}

\arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices
    specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
  \item{readXY}{If \code{\link[base:logical]{TRUE}}, cell row and column (x,y) coordinates are
     retrieved, otherwise not.}
  \item{readBases}{If \code{\link[base:logical]{TRUE}}, cell P and T bases are retrieved, otherwise not.}
  \item{readExpos}{If \code{\link[base:logical]{TRUE}}, cell "expos" values are retrieved, otherwise not.}
  \item{readType}{If \code{\link[base:logical]{TRUE}}, unit types are retrieved, otherwise not.}
  \item{readDirection}{If \code{\link[base:logical]{TRUE}}, unit directions are retrieved, otherwise not.}
  \item{stratifyBy}{A \code{\link[base]{character}} string specifying which and how
    elements in group fields are returned.
    If \code{"nothing"}, elements are returned as is, i.e. as \code{\link[base]{vector}}s.
    If \code{"pm"}/\code{"mm"}, only elements corresponding to 
    perfect-match (PM) / mismatch (MM) probes are returned (as \code{\link[base]{vector}}s).
    If \code{"pmmm"}, elements are returned as a matrix where the
    first row holds elements corresponding to PM probes and the second
    corresponding to MM probes.  Note that in this case, it is assumed 
    that there are equal number of PMs and MMs; if not, an error is
    generated.  
    Moreover, the PMs and MMs may not even be paired, i.e. there is no 
    guarantee that the two elements in a column corresponds to a 
    PM-MM pair.}
  \item{readIndices}{If \code{\link[base:logical]{TRUE}}, cell indices calculated from the row and column (X,Y) coordinates are retrieved, otherwise not.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
}

\value{
  A named \code{\link[base]{list}} where the names corresponds to the names
  of the units read.  Each element of the list is in turn a
  \code{\link[base]{list}} structure with three components:
  \item{groups}{A \code{\link[base]{list}} with one component for each group 
   (also called block). The information on each group is a 
   \code{\link[base]{list}} with five components: \code{x}, \code{y}, 
   \code{pbase}, \code{tbase}, \code{expos}, and \code{indices}.}
  \item{type}{An \code{\link[base]{integer}} specifying the type of the
    unit, where 1 is "expression", 2 is "genotyping", 3 is "CustomSeq", 
    and 4 "tag".}
  \item{direction}{An \code{\link[base]{integer}} specifying the direction
    of the unit, which defines if the probes are interrogating the sense
    or the anti-sense target, where 0 is "no direction", 1 is "sense", and
    2 is "anti-sense".}
}

\author{
 James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
 Daniel Hansen, \email{khansen@stat.berkeley.edu}.
 Modified by Henrik Bengtsson (\url{http://www.braju.com/R/}) to read
 any subset of units and/or subset of parameters, to stratify by PM/MM,
 and to return cell indices.
}

\examples{
for (zzz in 0) {

# Find any CDF file
cdfFile <- findCdf()
if (is.null(cdfFile))
  break

# Read all units in a CDF file [~20s => 0.34ms/unit]
cdf0 <- readCdfUnits(cdfFile)

# Read a subset of units in a CDF file [~6ms => 0.06ms/unit]
units1 <- c(5, 100:109, 34)
cdf1 <- readCdfUnits(cdfFile, units=units1)
stopifnot(identical(cdf1, cdf0[units1]))
rm(cdf0)

# Create a unit name to index map
names <- readCdfUnitNames(cdfFile)
units2 <- match(names(cdf1), names)
stopifnot(identical(units1, units2))
cdf2 <- readCdfUnits(cdfFile, units=units2)

stopifnot(identical(cdf1, cdf2))


# Clean up
rm(cdfFile, units1, cdf1, names, units2, cdf2)
} # for (zzz in 0)
}


 \seealso{
   To read unit names only, see \code{\link{readCdfUnitNames}}().
   To get which features are perfect-match probes and not, see
   \code{\link{readCdfIsPm}}().
 }

 \references{
   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
       June 14, 2005.
       \url{http://www.affymetrix.com/support/developer/}
 }



\keyword{file}
\keyword{IO}

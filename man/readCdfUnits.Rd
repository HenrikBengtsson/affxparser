
\name{readCdfUnits}
\alias{readCdfUnits}

\title{Reads units (probesets) from an Affymetrix CDF file}

 \usage{readCdfUnits(filename, units=NULL, readXY=TRUE, readBases=TRUE, readExpos=TRUE, readType=TRUE, readDirection=TRUE, splitPmMm=FALSE, verbose=0)}

\description{
  Gets the all or a subset of units (probesets) in an Affymetrix CDF file.
}

\arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of (zero-based) unit indices
    specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
  \item{readXY}{If \code{\link[base:logical]{TRUE}}, cell row and column (X,Y) coordinates are
     retrieved, otherwise not.}
  \item{readBases}{If \code{\link[base:logical]{TRUE}}, cell P and T bases are retrieved, otherwise not.}
  \item{readExpos}{If \code{\link[base:logical]{TRUE}}, cell "expos" values are retrieved, otherwise not.}
  \item{readType}{If \code{\link[base:logical]{TRUE}}, unit types are retrieved, otherwise not.}
  \item{readDirection}{If \code{\link[base:logical]{TRUE}}, unit directions are retrieved, otherwise not.}
  \item{splitPmMm}{If \code{\link[base:logical]{TRUE}}, each of the cell data elements are
     returned as a two-row matrix where the first row corresponds to
     perfect-match (PM) features and the second to mismatch (MM) features.
     Note that this assumes that there are equal number of PMs and MMs.
     Note also that it is assumed that the PMs and MMs are ordered in the
     CDF file such that first MM corresponds to the first PM and so on.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
}

\value{
  A named \code{\link[base]{list}} where the names corresponds to the names of the units
  read.  Each element of the list is in turn a \code{\link[base]{list}} structure
  with three components:
  \item{groups}{A \code{\link[base]{list}} with one component for each group (also called
    block). The information on each group is a \code{\link[base]{list}} with five
    components: \code{x}, \code{y}, \code{pbase}, \code{tbase},
    \code{expos}.}
    \item{type}{An \code{\link[base]{integer}} specifying the type of the unit.}
    \item{direction}{An \code{\link[base]{integer}} specifying the direction of the unit.}
  }

  \author{James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
    Daniel Hansen, \email{khansen@stat.berkeley.edu}.
   Modified by Henrik Bengtsson, \email{hb@maths.lth.se}, to read
   any subset of units and/or subset of parameters, and to split by
   PM and MM.}
 
 \examples{\dontrun{
filename <- "Mapping50K_Xba240.CDF"

# Read all units in a CDF file [~20s => 0.34ms/unit]
cdf0 <- readCdfUnits(filename)

# Read a subset of units in a CDF file [~6ms => 0.06ms/unit]
units1 <- c(5, 100:109, 34, 50400:50487)
cdf1 <- readCdfUnits(filename, units=units1)
stopifnot(identical(cdf1, cdf0[units1+1]))

# Create a unit name to index map
names <- readCdfUnitNames(filename)
units2 <- match(names(cdf1), names) - 1
stopifnot(identical(units1, units2))
cdf2 <- readCdfUnits(filename, units=units2)

stopifnot(identical(cdf1, cdf2))
}}

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

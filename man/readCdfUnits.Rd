
\name{readCdfUnits}
\alias{readCdfUnits}

\title{Reads units (probesets) from an Affymetrix CDF file}

\usage{readCdfUnits(filename, units=NULL, verbose=0)}

\description{
  Gets the all or a subset of units (probesets) in an Affymetrix CDF file.
}

\arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of (zero-based) unit indices
    specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
}

\value{
  A named \code{\link[base]{list}} where the names corresponds to the names of the units
  read.  Each element of the \code{\link[base]{list}} is in turn a \code{\link[base]{list}} structure
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
   any subset of units.}
 
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
   \code{\link{readCdfUnitNames}}().
 }

 \references{
   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
       June 14, 2005.
       \url{http://www.affymetrix.com/support/developer/}
 }



\keyword{file}
\keyword{IO}

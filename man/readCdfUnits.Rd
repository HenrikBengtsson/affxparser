%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdfUnits.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCdfUnits}
\alias{readCdfUnits}


 \title{Reads units (probesets) from an Affymetrix CDF file}

 \usage{
readCdfUnits(filename, units=NULL, readXY=TRUE, readBases=TRUE, readExpos=TRUE,
  readType=TRUE, readDirection=TRUE, stratifyBy=c("nothing", "pmmm", "pm", "mm"),
  readIndices=FALSE, verbose=0)
}

 \description{
  Reads units (probesets) from an Affymetrix CDF file. Gets all or a subset of units (probesets).
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
  \item{readDirection}{If \code{\link[base:logical]{TRUE}}, unit \emph{and} group directions are
    retrieved, otherwise not.}
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
  \item{readIndices}{If \code{\link[base:logical]{TRUE}}, cell indices \emph{calculated} from
    the row and column (x,y) coordinates are retrieved, otherwise not.
     Note that these indices are \emph{one-based}.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
 }

 \value{
  A named \code{\link[base]{list}} where the names corresponds to the names
  of the units read.  Each element of the list is in turn a
  \code{\link[base]{list}} structure with three components:
  \item{groups}{A \code{\link[base]{list}} with one component for each group
   (also called block). The information on each group is a
   \code{\link[base]{list}} of up to seven components: \code{x}, \code{y},
   \code{pbase}, \code{tbase}, \code{expos}, \code{indices},
   and \code{direction}.
   All fields but the latter have the same number of values as
   there are cells in the group.  The latter field has only
   one value indicating the direction for the whole group.
  }
  \item{type}{An \code{\link[base]{integer}} specifying the type of the
    unit, where 1 is "expression", 2 is "genotyping", 3 is "CustomSeq",
    and 4 "tag".}
  \item{direction}{An \code{\link[base]{integer}} specifying the direction
    of the unit, which defines if the probes are interrogating the sense
    or the anti-sense target, where 0 is "no direction", 1 is "sense", and
    2 is "anti-sense".}
 }

 \section{Cell indices are one-based}{
   Note that in \pkg{affxparser} all \emph{cell indices} are by
   convention \emph{one-based}, which is more convenient to work
   with in \R.  For more details on one-based indices, see
   \code{\link{2. Cell coordinates and cell indices}}.
 }

 \author{
  James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
  Daniel Hansen, \email{khansen@stat.berkeley.edu}.
  Modified by Henrik Bengtsson (\url{http://www.braju.com/R/}) to read
  any subset of units and/or subset of parameters, to stratify by PM/MM,
  and to return cell indices.d
 }

 \examples{
##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Find any CDF file
cdfFile <- findCdf()

# Read all units in a CDF file [~20s => 0.34ms/unit]
cdf0 <- readCdfUnits(cdfFile, readXY=FALSE, readExpos=FALSE)

# Read a subset of units in a CDF file [~6ms => 0.06ms/unit]
units1 <- c(5, 100:109, 34)
cdf1 <- readCdfUnits(cdfFile, units=units1, readXY=FALSE, readExpos=FALSE)
stopifnot(identical(cdf1, cdf0[units1]))
rm(cdf0)

# Create a unit name to index map
names <- readCdfUnitNames(cdfFile)
units2 <- match(names(cdf1), names)
stopifnot(all.equal(units1, units2))
cdf2 <- readCdfUnits(cdfFile, units=units2, readXY=FALSE, readExpos=FALSE)

stopifnot(identical(cdf1, cdf2))

##############################################################
}                                                     # STOP #
##############################################################

}

 \seealso{
   \code{\link{readCdfCellIndices}}().
 }

 \references{
   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
       June 14, 2005.
       \url{http://www.affymetrix.com/support/developer/}
 }



\keyword{file}
\keyword{IO}

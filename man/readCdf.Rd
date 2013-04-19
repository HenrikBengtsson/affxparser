%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdf.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCdf}
\alias{readCdf}


 \title{Parsing a CDF file using Affymetrix Fusion SDK}

 \description{
  Parsing a CDF file using Affymetrix Fusion SDK.
  This function parses a CDF file using the Affymetrix Fusion SDK.
  \emph{This function will most likely be replaced by the more
        general \code{\link{readCdfUnits}()} function.}
 }

 \usage{
  readCdf(filename, units=NULL,
          readXY=TRUE, readBases=TRUE,
          readIndexpos=TRUE, readAtoms=TRUE,
          readUnitType=TRUE, readUnitDirection=TRUE,
          readUnitNumber=TRUE, readUnitAtomNumbers=TRUE,
          readGroupAtomNumbers=TRUE, readGroupDirection=TRUE,
          readIndices=FALSE, readIsPm=FALSE,
          stratifyBy=c("nothing", "pmmm", "pm", "mm"),
          verbose=0)
 }

 \arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices
    specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
  \item{readXY}{If \code{\link[base:logical]{TRUE}}, cell row and column (x,y) coordinates are
     retrieved, otherwise not.}
  \item{readBases}{If \code{\link[base:logical]{TRUE}}, cell P and T bases are retrieved, otherwise not.}
  \item{readIndexpos}{If \code{\link[base:logical]{TRUE}}, cell indexpos are retrieved, otherwise not.}
  \item{readExpos}{If \code{\link[base:logical]{TRUE}}, cell "expos" values are retrieved, otherwise not.}
  \item{readUnitType}{If \code{\link[base:logical]{TRUE}}, unit types are retrieved, otherwise not.}
  \item{readUnitDirection}{If \code{\link[base:logical]{TRUE}}, unit directions are retrieved, otherwise not.}
  \item{readUnitNumber}{If \code{\link[base:logical]{TRUE}}, unit numbers are retrieved, otherwise not.}
  \item{readUnitAtomNumbers}{If \code{\link[base:logical]{TRUE}}, unit atom numbers are retrieved, otherwise not.}
  \item{readGroupAtomNumbers}{If \code{\link[base:logical]{TRUE}}, group atom numbers are retrieved, otherwise not.}
  \item{readGroupDirection}{If \code{\link[base:logical]{TRUE}}, group directions are retrieved, otherwise not.}
  \item{readIndices}{If \code{\link[base:logical]{TRUE}}, cell indices \emph{calculated} from
    the row and column (x,y) coordinates are retrieved, otherwise not.
     Note that these indices are \emph{one-based}.}
  \item{readIsPm}{If \code{\link[base:logical]{TRUE}}, cell flags indicating whether the cell
    is a perfect-match (PM) probe or not are retrieved, otherwise not.}
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
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
 }

 \value{
   A list with one component for each unit. Every component is again a
   list with three components  
   \item{groups}{This is again a list with one component for each group
     (also called block). The information on each group is a list with 5
     components, \code{x}, \code{y}, \code{pbase}, \code{tbase},
   \code{expos}.}
   \item{type}{type of the unit.}
   \item{direction}{direction of the unit.}
 }

 \note{
   This version of the function does not return information on the QC
   probes. This will be added in a (near) future release. In addition we
   expect the header to be part of the returned object.

   So expect changes to the structure of the value of the function in
   next release. Please contact the developers for details.
 }

 \section{Cell indices are one-based}{
   Note that in \pkg{affxparser} all \emph{cell indices} are by
   convention \emph{one-based}, which is more convenient to work
   with in \R.  For more details on one-based indices, see
   \code{\link{2. Cell coordinates and cell indices}}.
 }

 \author{
  James Bullard, \email{bullard@stat.berkeley.edu} and
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}.
 }

 \seealso{
   It is recommended to use \code{\link{readCdfUnits}}() instead of this method.
   \code{\link{readCdfHeader}}() for getting the header of a CDF file.
 }

 \references{
   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
       June 14, 2005.
       \url{http://www.affymetrix.com/support/developer/}
 }




\keyword{file}
\keyword{IO}
\keyword{internal}

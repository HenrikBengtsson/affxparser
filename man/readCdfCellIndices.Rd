%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdfCellIndices.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCdfCellIndices}
\alias{readCdfCellIndices}


 \title{Reads cell indices of units (probesets) in an Affymetrix CDF file}

 \usage{readCdfCellIndices(filename, units=NULL, stratifyBy=c("nothing", "pmmm", "pm", "mm"), flat=FALSE, verbose=0)}

 \description{
   Reads cell indices of units (probesets) in an Affymetrix CDF file.
 }

 \arguments{
   \item{filename}{The filename of the CDF file.}
   \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices
     specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
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

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}

  \seealso{
    \code{\link{readCdfUnits}}().
  }



\keyword{file}
\keyword{IO}

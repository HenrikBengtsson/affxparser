%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  
% 
% on Wed Mar 29 20:22:52 2006.
% 
% Generator was the Rdoc class, which is part of the R.oo package written
% by Henrik Bengtsson, 2001-2006.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCelUnits}
\alias{readCelUnits}


 \title{Reads probe-level data ordered as units (probesets) from one or several Affymetrix CEL files}

 \usage{readCelUnits(filenames, units=NULL, ..., transforms=NULL, cdf=NULL, stratifyBy=c("nothing", "pmmm", "pm", "mm"), addDimnames=FALSE, readMap=NULL, dropArrayDim=TRUE, verbose=FALSE)}

 \description{
   Reads probe-level data ordered as units (probesets) from one or several Affymetrix CEL files by using the unit and group definitions in the
   corresponding Affymetrix CDF file.
 }

 \arguments{
   \item{filenames}{The filenames of the CEL files.}
   \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices specifying which
     units to be read.  If \code{\link[base]{NULL}}, all units are read.}
   \item{...}{Arguments passed to low-level method
     \code{\link[affxparser]{readCel}}, e.g. \code{readXY} and \code{readStdvs}.}
   \item{transforms}{A \code{\link[base]{list}} of exactly \code{length(filenames)}
     \code{\link[base]{function}}s.  If \code{\link[base]{NULL}}, no transformation is performed.
     Intensities read are passed through the corresponding transform
     function before being returned.}
   \item{cdf}{A \code{\link[base]{character}} filename of a CDF file, or a CDF \code{\link[base]{list}}
     structure.  If \code{\link[base]{NULL}}, the CDF file is searched for by
     \code{\link{findCdf}}() first starting from the current directory and
     then from the directory where the first CEL file is.}
   \item{stratifyBy}{Argument passed to low-level method
     \code{\link[affxparser]{readCdfUnits}}.}
   \item{addDimnames}{If \code{\link[base:logical]{TRUE}}, dimension names are added to arrays,
     otherwise not.  The size of the returned CEL structure in bytes
     increases by 30-40\% with dimension names.}
   \item{readMap}{A \code{\link[base]{vector}} remapping cell indices to file indices.
     If \code{\link[base]{NULL}}, no mapping is used.}
   \item{dropArrayDim}{If \code{\link[base:logical]{TRUE}} and only one array is read, the elements of
     the group field do \emph{not} have an array dimension.}
   \item{verbose}{Either a \code{\link[base]{logical}}, a \code{\link[base]{numeric}}, or a \code{\link[R.utils]{Verbose}}
     object specifying how much verbose/debug information is written to
     standard output. If a Verbose object, how detailed the information is
     is specified by the threshold level of the object. If a numeric, the
     value is used to set the threshold of a new Verbose object. If \code{\link[base:logical]{TRUE}},
     the threshold is set to -1 (minimal). If \code{\link[base:logical]{FALSE}}, no output is written
     (and neither is the \pkg{R.utils} package required).
   }
 }

 \value{
   A named \code{\link[base]{list}} with one element for each unit read.  The names
   corresponds to the names of the units read.
   Each unit element is in
   turn a \code{\link[base]{list}} structure with groups (aka blocks).
   Each group contains requested fields, e.g. \code{intensities},
   \code{stdvs}, and \code{pixels}.
   If more than one CEL file is read, an extra dimension is added
   to each of the fields corresponding, which can be used to subset
   by CEL file.

   Note that neither CEL headers nor information about outliers and
   masked cells are returned.  To access these, use \code{\link{readCelHeader}}()
   and \code{\link{readCel}}().
 }

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}

 \examples{

for (zzz in 0) {

# Scan current directory for CEL files
celFiles <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
if (length(celFiles) == 0)
  break

units <- 100:261
t <- system.time({
cel <- readCelUnits(celFiles, units=units, readPixels=TRUE, readStdvs=TRUE)
})
print(cel[1:2])

cat(sprintf("Time per unit and array: \%.4fms\n",
                                   1000*t[3]/length(cel)/length(celFiles)))

# Clean up
rm(celFiles, cel, t)

} # for (zzz in 0)
}

 \seealso{
   Internally, \code{\link{readCelHeader}}(), \code{\link{readCdfUnits}}() and
   \code{\link{readCel}}() are used.
 }

 \references{
   [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
       June 14, 2005.
       \url{http://www.affymetrix.com/support/developer/}
 }



\keyword{file}
\keyword{IO}

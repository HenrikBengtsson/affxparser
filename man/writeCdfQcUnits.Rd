%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  writeCdfQcUnits.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{writeCdfQcUnits}
\alias{writeCdfQcUnits}


 \title{Writes CDF QC units}

 \usage{
writeCdfQcUnits(con, cdfQcUnits, verbose=0)
}

 \description{
   Writes CDF QC units.
   \emph{This method is not indended to be used explicitly.
   To write a CDF, use \code{\link{writeCdf}}() instead.}
 }

 \arguments{
   \item{con}{An open \code{\link[base:connections]{connection}} to which a CDF header already has
     been written by \code{\link{writeCdfHeader}}().}
   \item{cdfQcUnits}{A \code{\link[base]{list}} structure of CDF QC units as returned by
     \code{\link{readCdf}}() (\emph{not} \code{\link{readCdfUnits}}()).}
   \item{verbose}{An \code{\link[base]{integer}} specifying how much verbose details are
     outputted.}
 }

 \value{
   Returns nothing.
 }

 \author{Henrik Bengtsson}

 \seealso{
   This method is called by \code{\link{writeCdf}}().
   See also \code{\link{writeCdfHeader}}() and \code{\link{writeCdfUnits}}().
 }




\keyword{file}
\keyword{IO}
\keyword{internal}

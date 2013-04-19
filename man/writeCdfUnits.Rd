%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  writeCdfUnits.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{writeCdfUnits}
\alias{writeCdfUnits}


 \title{Writes CDF units}

 \usage{
writeCdfUnits(con, cdfUnits, verbose=0)
}

 \description{
   Writes CDF units.
   \emph{This method is not indended to be used explicitly.
   To write a CDF, use \code{\link{writeCdf}}() instead.}
 }

 \arguments{
   \item{con}{An open \code{\link[base:connections]{connection}} to which a CDF header and QC units
     already have been written by \code{\link{writeCdfHeader}}() and
     \code{\link{writeCdfQcUnits}}(), respectively.}
   \item{cdfUnits}{A \code{\link[base]{list}} structure of CDF units as returned by
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
   See also \code{\link{writeCdfHeader}}() and \code{\link{writeCdfQcUnits}}().
 }




\keyword{file}
\keyword{IO}
\keyword{internal}

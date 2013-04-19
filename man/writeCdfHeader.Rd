%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  writeCdfHeader.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{writeCdfHeader}
\alias{writeCdfHeader}


 \title{Writes a CDF header}

 \usage{
writeCdfHeader(con, cdfHeader, unitNames, qcUnitLengths, unitLengths, verbose=0)
}

 \description{
   Writes a CDF header.
   \emph{This method is not indended to be used explicitly.
   To write a CDF, use \code{\link{writeCdf}}() instead.}
 }

 \arguments{
   \item{con}{An open \code{\link[base:connections]{connection}} to which nothing has been written.}
   \item{cdfHeader}{A CDF header \code{\link[base]{list}} structure.}
   \item{unitNames}{A \code{\link[base]{character}} \code{\link[base]{vector}} of all unit names.}
   \item{qcUnitLengths}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of all the number of bytes
     in each of the QC units.}
   \item{unitLengths}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of all the number of bytes
     in each of the (ordinary) units.}
   \item{verbose}{An \code{\link[base]{integer}} specifying how much verbose details are
     outputted.}
 }

 \value{
   Returns nothing.
 }

 \author{Henrik Bengtsson}

 \seealso{
   This method is called by \code{\link{writeCdf}}().
   See also \code{\link{writeCdfQcUnits}}() and \code{\link{writeCdfUnits}}().
 }




\keyword{file}
\keyword{IO}
\keyword{internal}

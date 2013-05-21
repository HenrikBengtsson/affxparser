%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCcgHeader.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCcgHeader}
\alias{readCcgHeader}


 \title{Reads an the header of an Affymetrix Command Console Generic (CCG) file}

 \usage{
readCcgHeader(pathname, verbose=0, .filter=list(fileHeader = TRUE, dataHeader = TRUE),
  ...)
}

 \description{
   Reads an the header of an Affymetrix Command Console Generic (CCG) file.
 }

 \arguments{
   \item{pathname}{The pathname of the CCG file.}
   \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
     file is parsed quietly.  The higher numbers, the more details.}
   \item{.filter}{A \code{\link[base]{list}}.}
   \item{...}{Not used.}
 }

 \value{
   A named \code{\link[base]{list}} structure consisting of ...
 }

 \author{Henrik Bengtsson}

  \details{
    Note, the current implementation of this methods does not utilize the
    Affymetrix Fusion SDK library.  Instead, it is implemented in R from the
    file format definition [1].
  }

  \seealso{
    \code{\link{readCcg}}().
  }

 \references{
  [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
      April, 2006.
      \url{http://www.affymetrix.com/support/developer/}\cr
 }



\keyword{file}
\keyword{IO}

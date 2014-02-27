%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCcg.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{readCcg}
\alias{readCcg}


\title{Reads an Affymetrix Command Console Generic (CCG) Data file}

\usage{
readCcg(pathname, verbose=0, .filter=NULL, ...)
}

\description{
  Reads an Affymetrix Command Console Generic (CCG) Data file.  The CCG data file format is also known as the
  Calvin file format.
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

 \details{
   Note, the current implementation of this methods does not utilize the
   Affymetrix Fusion SDK library.  Instead, it is implemented in R from the
   file format definition [1].
 }

\section{About the CCG file format}{
 A CCG file, consists of a "file header", a "generic data header",
 and "data" section, as outlined here:
 \itemize{
  \item File Header
  \item Generic Data Header (for the file)
   \enumerate{
    \item Generic Data Header (for the files 1st parent)
     \enumerate{
      \item Generic Data Header (for the files 1st parents 1st parent)
      \item Generic Data Header (for the files 1st parents 2nd parent)
      \item ...
      \item Generic Data Header (for the files 1st parents Mth parent)
     }
   \item Generic Data Header (for the files 2nd parent)
   \item ...
   \item Generic Data Header (for the files Nth parent)
  }
  \item Data
   \enumerate{
    \item Data Group \#1
     \enumerate{
      \item Data Set \#1
       \itemize{
        \item Parameters
        \item Column definitions
        \item Matrix of data
       }
      \item Data Set \#2
      \item ...
      \item Data Set \#L
     }
    \item Data Group \#2
    \item ...
    \item Data Group \#K
   }
 }
}

\author{Henrik Bengtsson}

 \seealso{
   \code{\link{readCcgHeader}}().
   \code{\link{readCdfUnits}}().
 }

\references{
 [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
     April, 2006.
     \url{http://www.affymetrix.com/support/developer/}\cr
}



\keyword{file}
\keyword{IO}

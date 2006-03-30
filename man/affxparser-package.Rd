 \name{affxparser-package}
\alias{affxparser-package}
\alias{affxparser}
\docType{package}

\title{Package affxparser}

\description{
  The \pkg{affxparser} package provides methods for fast and memory
  efficient parsing of Affymetrix files [1] using the Affymetrix' 
  Fusion SDK [2].  Both ASCII- and binary-based files are supported.
  
  Currently, there are methods for reading chip definition file (CDF) 
  and a cell intensity file (CEL).  These files can be read either in 
  full or in part.  For example, probe signals from a few probesets 
  can be extracted very quickly from a set of CEL files into a 
  convenient list structure.
}

\section{Requirements}{
  This package requires only a standard \R installation, that is,
  it works independently of other CRAN and Bioconductor packages.

  Except from the Solaris platform, which we are currently working 
  on to also support, this package has been confirmed to work on 
  Microsoft Windows as well as several Unix-like systems.
}

\section{Installation}{
  To install this package, see \url{http://www.bioconductor.org/}.
}

\section{To get started}{
  To get started, see:
  \enumerate{
    \item \code{\link{readCelUnits}}() - reads one or several Affymetrix CEL file probeset by probeset.
    \item \code{\link{readCel}}() - reads an Affymetrix CEL file probe by probe.
    \item \code{\link{findCdf}}() - Locates an Affymetrix CDF file by chip type.
    \item \code{\link{readCdfUnits}}() - reads an Affymetrix CDF file unit by unit.
    \item \code{\link{applyCdfGroups}}() - Re-arranges a CDF structure.
  }
}

\section{Running examples}{
  In order to run the examples, data files must exists in the current directory.  Otherwise, the example scripts will do nothing.  Most of the examples requires a CDF file or a CEL file, or both.  Make sure the CDF file is of the same chip type as the CEL file.

  Affymetrix provides data sets of different types at \url{http://www.affymetrix.com/support/datasets.affx} that can be used.  There are both small are very large data sets available.
}

\author{
 James Bullard, \email{bullard@stat.berkeley.edu} and 
 Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}.
}

\section{License}{
  The releases of this package is licensed under LGPL version 2.1 or newer.
}

\references{
  [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
      June 14, 2005.
      \url{http://www.affymetrix.com/support/developer/}\cr
  [2] Affymetrix Inc, Fusion Software Developers Kit (SDK), 2005.
      \url{http://www.affymetrix.com/support/developer/fusion/}\cr
}

\keyword{package}

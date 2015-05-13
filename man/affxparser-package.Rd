\name{affxparser-package}
\alias{affxparser-package}
\alias{affxparser}
\docType{package}

\title{Package affxparser}

\description{
  The \pkg{affxparser} package provides methods for fast and memory
  efficient parsing of Affymetrix files [1] using the Affymetrix' 
  Fusion SDK [2].  Both traditional ASCII- and binary (XDA)-based files are
  supported, as well as Affymetrix future binary format "Calvin". The
  efficiency of the parsing is dependent on whether a specific file is
  binary or ASCII.
  
  Currently, there are methods for reading chip definition file (CDF) 
  and a cell intensity file (CEL).  These files can be read either in 
  full or in part.  For example, probe signals from a few probesets 
  can be extracted very quickly from a set of CEL files into a 
  convenient list structure.
}

\section{To get started}{
  To get started, see:
  \enumerate{
    \item \code{\link{readCelUnits}}() - reads one or several Affymetrix
  CEL file probeset by probeset. 
    \item \code{\link{readCel}}() - reads an Affymetrix CEL file.
    by probe.
    \item \code{\link{readCdf}}() - reads an Affymetrix CDF file.
    by probe.
    \item \code{\link{readCdfUnits}}() - reads an Affymetrix CDF file unit by unit. 
    \item \code{\link{readCdfCellIndices}}() - Like \code{readCdfUnits()}, but returns cell indices only, which is often enough to read CEL files unit by unit.
    \item \code{\link{applyCdfGroups}}() - Re-arranges a CDF structure.
    \item \code{\link{findCdf}}() - Locates an Affymetrix CDF file by chip type.  This page also describes how to setup default search path for CDF files.
  }
}

\section{Setting up the CDF search path}{
Some of the functions in this package search for CDF files automatically by scanning certain directories.  To add directories to the default search path, see instructions in \code{\link{findCdf}}().
}

\section{Future Work}{
  Other Affymetrix files can be parsed using the Fusion SDK. Given
  sufficient interest we will implement this, e.g. DAT files (image files).
}

\section{Running examples}{
  In order to run the examples, data files must exists in the current
  directory.  Otherwise, the example scripts will do nothing.  Most of
  the examples requires a CDF file or a CEL file, or both.  Make sure
  the CDF file is of the same chip type as the CEL file. 

  Affymetrix provides data sets of different types at
  \url{http://www.affymetrix.com/support/datasets.affx} that can be
  used.  There are both small are very large data sets available. 
}

\section{Tecnical details}{
  This package implements an interface to the Fusion SDK from
  Affymetrix.com. This SDK (software development kit) is an open source
  library used for parsing the various files formats used by the
  Affymetrix platform.

  The intention is to provide interfaces to most if not all file formats
  which may be parsed using Fusion.

  The SDK supports parsing of all the different versions of a specific
  fileformat. This means that ASCII, binary as well as the new binary
  format (codename Calvin) used by Affymetrix is supported through a
  single API. We also expect any future changes to the file formats to
  be reflected in the SDK, and subsequently in this package.

  However, as the current Fusion SDK does not support compressed files,
  neither does \pkg{affxparser}. This is in contrast to some of the
  existing code in \pkg{affy} and relatives (see below for links).

  In general we aim to provide functions returning all information in
  the respective files. Currently it seems that future Affymetrix chip
  designs may consists of so many features that returning all
  information will lead to an unnecessary overhead in the case a user
  only wants access to a subset. We have tried to make this possible.

  For older file, certain entries in the files have been removed from
  newer specifications, and the SDK does not provide utilities for
  reading these entries. This includes eg. the FEAT column of CDF files.
  
  Currently the package as well as the Fusion SDK is in beta stage. Bugs
  may be related to either codebase. We are very interested in users
  being unable to compile/parse files using this library - this includes
  users with custom chip designs.

  In addition, since we aim to return all information stored in the
  file (and accessible using the Fusion SDK) we would like reports from
  users being unable to do that.

  The efficiency of the underlying code may vary with the version of the
  file being parsed. For example, we currently report the number of
  outliers present in a CEL file when reading the header of the file
  using \code{readCelHeader}. In order to obtain this information
  from text based CEL files (version 2), the entire file needs to be
  read into memory. With version 3 of the file format, this information
  is stored in the header.
  
  With the introduction of the Fusion SDK (and the next version of their
  file formats) Affymetrix has made it possible to use multibyte
  character sets. This implies that character information may be
  inaccesible if the compiler used to compile the C++ code does not
  support multibyte character sets (specifically we require that the R
  installation has defined the macro \code{SUPPORT_MCBS} in the
  \code{Rconfig.h} header file). For example GCC needs to be version 3.4
  or greater on Solaris.

  In the \code{info} subdirectory of the package installation,
  information regarding changes to the Fusion SDK is stored, e.g.
  \preformatted{
    pathname <- system.file("info", "changes2fusion.txt", package="affxparser")
    file.show(pathname)
  }
}  
  
\author{
  Henrik Bengtsson [aut], James Bullard [aut], Robert Gentleman [ctb], Kasper Daniel Hansen [aut, cre], Martin Morgan [ctb]
}

\section{Acknowledgments}{
 We would like to thanks Ken Simpson (WEHI, Melbourne) and
 Seth Falcon (FHCRC, Seattle) for feedback and code contributions.
}

\section{License}{
  The releases of this package is licensed under LGPL version 2.1 or
  newer. This applies also to the Fusion SDK.
}

\references{
  [1] Affymetrix Inc, Affymetrix GCOS 1.x compatible file formats,
      April, 2006.
      \url{http://www.affymetrix.com/support/developer/}\cr
  [2] Affymetrix Inc, Fusion Software Developers Kit (SDK), 2006.
      \url{http://www.affymetrix.com/support/developer/fusion/}\cr
}

\keyword{package}

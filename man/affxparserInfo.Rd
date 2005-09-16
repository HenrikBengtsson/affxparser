\name{affxparserInfo}
\alias{affxparserInfo}
\title{Overview of the Affxparser package.}
\description{
  The package implements an interface to the Fusion SDK from
  Affymetrix.com.
}
\usage{
  affxparserInfo(fusion.license = FALSE, file.changes = FALSE)
}
\arguments{
  \item{fusion.license}{display the license of the Fusion SDK (this will
    happen in a browser).}
  \item{file.changes}{display the changes made to the Fusion SDK.}
}
\details{
  This package implements an interface to the Fusion SDK from
  Affymetrix.com. This SDK (software development kit) is an open source
  library used for parsing the various files formats used by the
  Affymetrix platform.

  The intention is to provide interfaces to most if not all file formats
  which may be parsed using Fusion.

  The SDK supports parsing of all the different versions of a specific
  fileformat. This means that ascii, binary as well as the new binary
  format (codename Calvin) used by Affymetrix is supported through a
  single API. We also expect any future changes to the file formats to
  be reflected in the SDK, and subsequently in this package.

  However, as the current Fusion SDK does not support compressed files,
  neither does \pkg{affxparser}. This is in contrast to some of the
  existing code in \pkg{affy} and relatives (see below for links).

  In general we aim to provide functions returning all information in
  the respective files. Currently it seems that future Affymetrix chip
  designs may consists of so many features that returning all
  information will lead to an unnecessary overhead on the case a user
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
  outliers present in a CEL file. In order to obtain this information
  from text based CEL files (version 2), the entire file needs to be
  read into memory. With version 3 of the file format, this information
  is stored in the header.
  
  With the introduction of the Fusion SDK (and the next version of their
  file formats) Affymetrix has made it possible to use multibyte
  character sets. This implies that character information may be
  inaccesible if the compiler used to compile the c++ code does not
  support multibyte character sets (specifically we require that the R
  installation has defined the macro \code{SUPPORT_MCBS} in the
  \code{Rconfig.h} header file).
  
  The \code{affxparserInfo} function returns relevant information
  regarding the version of the Fusion SDK (and possibly more). It simply
  echoes the README file in the package directory.
}  
\author{James bullard, \email{bullard@stat.berkeley.edu} and
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}}
\references{\url{http://www.affymetrix.com}, particular the support and
  developer parts of the website.}
\seealso{
  Existing functions for parsing Affymetrix files may be found in
  \pkg{affy}, \pkg{oligo}, \pkg{makecdfenv} and
  \pkg{makePlatformDesign}.
  }
\examples{
  affxparserInfo()
}
\keyword{file}

\name{affxparserInfo}
\alias{affxparserInfo}
\title{Overview of the Affxparser package.}
\description{
  The package implements an interface to the Fusion SDK from
  Affymetrix.com.
}
\usage{
  affxparserInfo()
}
\details{
  The package implements an interface to the Fusion SDK from
  Affymetrix.com. This SDK (software development kit) is an open source
  library used for parsing the various files formats used by the
  Affymetrix platform.

  The intention is to provide interfaces to all file formats which may
  be parsed using Fusion.

  In general we aim to provide functions returning all information in
  the respective files. Currently it seems that future Affymetrix chip
  may consists of so many features that returning all information will
  be inefficient. In that case we may provide specialized functions
  returning only parts of the file.

  Currently the package as well as the Fusion SDK is in beta stage. Bugs
  may be related to either codebase. We are very interested in users
  being unable to compile/parse files using this library - this includes
  users with custom chip designs.

  Since we aim to return all information obtainable from agiven file, it
  is a bug if that is not the case.

  The \code{affxparserInfo} function returns relevant information
  regarding the version of the Fusion SDK (and possibly more). It simply
  echoes the README file in the package directory.
}  
\author{James bullard <bullard@stat.berkeley.edu and
  Kasper Daniel Hansen <khansen@stat.berkeley.edu>}
\references{\url{http://www.affymetrix.com}, particular the support and
  developer parts of the website.}
\examples{
  affxparserInfo()
}
\keyword{file}

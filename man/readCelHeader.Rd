\name{readCelHeader}
\alias{readCelHeader}

\title{Parsing the header of an Affymetrix CEL file}

\description{
  Reads in the header of an Affymetrix CEL file using the Fusion SDK.
}

\usage{
readCelHeader(filename)
}

\arguments{
  \item{filename}{the name of the CEL file.}
}

\details{
  This function returns the header of a CEL file. Affymetrix operates
  with different versions of this file format. Depending on what version
  is being read, different information is accessible.
}

\value{
  A named list with components described below. The entries are
  obtained from the Fusion SDK interface functions. We try to obtain all
  relevant information from the file.
  \item{filename}{the name of the cel file.}
  \item{version}{the version of the cel file.}
  \item{cols}{the number of columns on the chip.}
  \item{rows}{the number of rows on the chip.}
  \item{total}{the total number of features on the chip. Usually equal
  to \code{rows} times \code{cols}, but since it is a separate attribute
  in the SDK we decided to include it anyway.}
  \item{algorithm}{the algorithm used to create the CEL file.}
  \item{parameters}{the parameters used in the algorithm. Seems to be
    semi-colon separated.}
  \item{chiptype}{the type of the chip.}
  \item{header}{the entire header of the CEL file. Only available for
    non-calvin format files.}
  \item{datheader}{the entire dat header of the CEL file. This contains
  for example a date.}
  \item{librarypackage}{the library package name of the file. Empty for
  older versions.}
  \item{cellmargin}{a parameter used to generate the CEL file. According
  to Affymetrix, it designates the number of pixels to ignore around the
  feature border when calculating the intensity value (the number of
  pixels ignored are cellmargin divided by 2).}
  \item{noutliers}{the number of features reported as outliers.}
  \item{nmasked}{the number of features reported as masked.}
}

\author{
  James Bullard, \email{bullard@stat.berkeley.edu} and
  Kasper Daniel Hansen, \email{khansen@stat.berkeley.edu}
}

\note{
  Memory usage:the Fusion SDK allocates memory for the entire CEL file,
  when the file is accessed. The memory footprint of this function will
  therefore seem to be (rather) large.

  Speed: CEL files of version 2 (standard text files) needs to be
  completely read in order to report the number of outliers and masked
  features.
}

\seealso{
  \code{\link{readCel}()} for reading in the entire CEL
  file. That function also returns the header. 
  See \code{affxparserInfo} for general comments on the package and
  the Fusion SDK.
}

\examples{
  # Scan current directory for CEL files
  files <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
  if (length(files) > 0) {
    header <- readCelHeader(files[1])
    print(header)
    rm(header)
  }

  # Clean up
  rm(files)
}

\keyword{file}
\keyword{IO}

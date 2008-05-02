\name{readChp}
\alias{readChp}
\title{ A function to read Affymetrix CHP files }
\description{
This function will parse any type of CHP file and return the results
in a list.  The contents of the list will depend on the type of CHP file
that is parsed and readers are referred to Affymetrix documentation of what
should be there, and how to interpret it.
}
\usage{
readChp(filename, withQuant = TRUE)
}
\arguments{
  \item{filename}{ The name of the CHP file to read. }
  \item{withQuant}{ A boolean value, currently largely unused. }
}
\details{
This is an interface to the Affymetrix Fusion SDK.  The Affymetrix documentation
should be consulted for explicit details.
}
\value{
A list is returned. The contents of the list depend on the type of CHP file 
that was read.  Users may want to translate the different outputs into
specific containers.
}
\section{Troubleshooting}{
  It is considered a bug if the file contains information not accessible
  by this function, please report it.
} 
\author{R. Gentleman}
\seealso{ \code{\link{readCel}}}
\examples{
if (require("AffymetrixDataTestFiles")) {
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](chp|CHP)$", path=path, 
                   recursive=TRUE, firstOnly=FALSE)

s1 = readChp(files[1])
length(s1)
names(s1)
names(s1[[7]])
}
}
\keyword{file}
\keyword{IO}

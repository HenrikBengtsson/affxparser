\name{readPgfHeader}
\alias{readPgfHeader}
\title{ Read the header of a PGF file into a list. }
\description{
  This function reads the header of a PGF file into a list more details
  on what the exact fields are can be found in the details section.
}
\usage{
readPgfHeader(file)
}
\arguments{
  \item{file}{\code{file}:A file in PGF format}
}
\details{
    \url{https://www.affymetrix.com/support/developer/fusion/File_Format_PGF_aptv161.pdf}
}
\value{
  A list corresponding to the elements in the header. 
}
\keyword{file}
\keyword{IO}

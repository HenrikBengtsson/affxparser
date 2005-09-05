\name{read.cel.complete}
\alias{read.cel.complete}
\title{Parsing an Affymetrix CEL file}
\description{
  Parsing an Affymetrix CEL file using the Fusion SDK. The 
  intention is to let users obtain all (but possibly a subset of)
  information in the CEL file.
}
\usage{
read.cel.complete(fname, indices = NULL, read.intensities = TRUE,
                  read.stdvs = TRUE, read.pixels = TRUE,
                  read.xy = FALSE, read.outliers = TRUE,
                  read.masked = TRUE, verbose = 0)
}
\arguments{
  \item{fname}{the name of the CEL file.}
  \item{indices}{a vector of indices indicating which features to
    read. If the argument is \code{NULL} all features will be returned.}
  \item{read.intensities}{a logical: will the intensities be returned.}
  \item{read.stdvs}{a logical: will the standard deviations be returned.}
  \item{read.pixels}{a logical: will the number of pixels be returned.}
  \item{read.xy}{a logical: will the (x,y) coordinates be returned.}
  \item{read.outliers}{a logical: will the outliers be return.}
  \item{read.masked}{a logical: will the masked features be returned.}
  \item{verbose}{how verbose do we want to be. 0 is no verbosity, higher
    numbers mean more verbose output. At the moment the values 1 and 2
    are supported.}
}
\details{
  This function reads everything from a CEL file. It is considered a bug
  if the file contains information not accessible by this function,
  please report it.

  The (input and output) interface makes heavily use of the concept of
  indices. An (x,y) coordinate is converted to an integer by the formula
  (y + 1) times numberOfColsInCelFile + (x + 1), assuming that the (x,y)
  coordinates start at (0,0).
  
  The file returns a list with components (possible empty
  depending on the input flags) corresponding to the various flags. The
  \code{outliers} and \code{masked} components are special in that they
  are vectors of indices of features flagged as respectively
  outlier/masked by the Affymetrix image software.
}
\value{
  A named list with components described
  below. The order of the \code{intensities}, \code{stdvs} and
  \code{pixels} corresponds to the ordering of the \code{indices}
  argument.
  \item{header}{The header of the CEL file. Equivalent to the output
  from \code{read.cel.header}, see the documentation for that function.}
  \item{intensities}{A vector of mode \code{nummeric} containing the
  intensity associated with each feature, unless \code{read.intensities
  = FALSE} in which case it will be \code{NULL}. Restricted to features 
  within the \code{indices} range.}
  \item{stdvs}{A vector of mode \code{nummeric} containing the
  standard deviation associated with each feature, unless
  \code{read.stdvs = FALSE} in which case it will be
  \code{NULL}. Restricted to features within the \code{indices} range.}
  \item{pixels}{A vector of mode \code{integer} containing the
  number of pixels associated with each feature, unless
  \code{read.pixels = FALSE} in which case it will be
  \code{NULL}. Restricted to features within the \code{indices} range.}
  \item{x}{A vector of mode \code{integer} containing the x coordinate
  associated with each feature, unless \code{read.xy = FALSE}
  in which case it will be \code{NULL}. Restricted to features with the
  \code{indices} range.}
  \item{y}{A vector of mode \code{integer} containing the y coordinate
  associated with each feature, unless \code{read.xy = FALSE}
  in which case it will be \code{NULL}. Restricted to features with the
  \code{indices} range.}
  \item{outliers}{A vector of mode \code{integer} which the indices of
  the features flagged as outliers, unless \code{read.outliers = NULL}
  in which case it will be \code{NULL}. Note that there is a difference
  between \code{outliers = NULL} and \code{outliers = integer(0)} - the
  last case happens when \code{read.outliers = TRUE} but there are no
  outliers. In case an \code{indices} argument is given, only indices
  within the \code{indices} argument are returned.}
  \item{masked}{A vector of mode \code{integer} which the indices of
  the features flagged as masked, unless \code{read.masked = NULL}
  in which case it will be \code{NULL}. Note that there is a difference
  between \code{masked = NULL} and \code{masked = integer(0)} - the
  last case happens when \code{read.masked = TRUE} but there are no
  masked features. In case an \code{indices} argument is given, only indices
  within the \code{indices} argument are returned.}   
}
\author{James Bullard \email{bullard@stat.berkeley.edu}, Kasper Daniel Hansen
 \email{khansen@stat.berkeley.edu}} 
\note{
  Memory usage: the Fusion SDK allocates memory for the entire
  CEL file, when the file is accessed (but does not actually read the
  file into memory). Using the \code{indices} argument will therefore
  only affect the memory use of the final object (as well as speed), not
  the memory allocated in the C function used to parse the file. This
  should be a minor problem however.
  
  The outlier/masked flag are computed by the Affymetrix image analysis
  software. The current community view seems to be that this should be
  done based on statistical modelling of the actual probe intensities
  and should be based on the choice of preprocessing algorithm. Mopst
  algorithms are only using the intensities from the CEL file.
}
\seealso{ \code{\link{affxparserInfo}} for a package overview and
  \code{\link{read.cel.header}} for a description of the header
  output. Often a user only wants to read the intensities, look at
  \code{\link{read.cel.intensities}} for a function specialized for that
  use. See \code{\link{xy2indices}} from \pkg{affy} and
  \code{\link{xy2i}} from \pkg{makePlatformDesign} for a discussion of
  the index approach.  }
\examples{
\dontrun{
read.cel.complete("~/testFile.CEL", read.npixels = FALSE)
}
}
\keyword{file}

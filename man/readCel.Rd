\name{readCel}
\alias{readCel}

\title{Reads an Affymetrix CEL file}

\description{
  This function reads all or a subset of the data in an Affymetrix 
  CEL file.
}

\usage{
readCel(filename, 
        indices = NULL, 
        readHeader = TRUE, 
        readXY = FALSE, readIntensities = TRUE,
        readStdvs = FALSE, readPixels = FALSE,
        readOutliers = TRUE, readMasked = TRUE, 
        readMap = NULL,
        verbose = 0,
        .checkArgs = TRUE)
}

\arguments{
  \item{filename}{the name of the CEL file.}
  \item{indices}{a vector of indices indicating which features to
    read. If the argument is \code{NULL} all features will be returned.}
  \item{readXY}{a logical: will the (x,y) coordinates be returned.}
  \item{readIntensities}{a logical: will the intensities be returned.}
  \item{readStdvs}{a logical: will the standard deviations be returned.}
  \item{readPixels}{a logical: will the number of pixels be returned.}
  \item{readOutliers}{a logical: will the outliers be return.}
  \item{readMasked}{a logical: will the masked features be returned.}
  \item{readHeader}{a logical: will the header of the file be returned.}
  \item{readMap}{A \code{\link[base]{vector}} remapping cell indices to 
     file indices.  If \code{\link[base]{NULL}}, no mapping is used.}
  \item{verbose}{how verbose do we want to be. 0 is no verbosity, higher
    numbers mean more verbose output. At the moment the values 0, 1 and 2
    are supported.}
  \item{.checkArgs}{If \code{TRUE}, the arguments will be validated,
    otherwise not.  \emph{Warning: This should only be used if the
    arguments have been validated elsewhere!}}
}

\value{
  A CEL files consists of a \emph{header}, a set of \emph{cell values},
  and information about \emph{outliers} and \code{masked} cells.

  The cell values, which are values extract for each cell (aka feature
  or probe), are the (x,y) coordinate, intensity and standard deviation
  estimates, and the number of pixels in the cell.  
  If \code{readIndices=NULL}, cell values for all cells are returned,
  Only cell values specified by argument \code{readIndices} are returned.

  This value returns a named list with compontents described below:

   \item{\code{header}}{The header of the CEL file. Equivalent to the 
      output from \code{readCelHeader}, see the documentation for that
      function.}

   \item{x,y}{(cell values) Two \code{integer} vectors containing
      the x and y coordinates associated with each feature.}

   \item{\code{intensities}}{(cell value) A  \code{numeric} vector
      containing the intensity associated with each feature.}

   \item{stdvs}{(cell value) A \code{numeric} vector containing 
      the standard deviation associated with each feature.}

   \item{pixels}{(cell value) An \code{integer} vector containing 
      the number of pixels associated with each feature.}

   \item{outliers}{An \code{integer} vector of indices specifying which
      of the queried cells that are flagged as outliers.
      Note that there is a difference between \code{outliers=NULL} and
      \code{outliers=integer(0)}; the last case happens when 
      \code{readOutliers=TRUE} but there are no outliers.}
 
   \item{masked}{An \code{integer} vector of indices specifying which
      of the queried cells that are flagged as masked.
      Note that there is a difference between \code{masked=NULL} and
      \code{masked=integer(0)}; the last case happens when 
      \code{readMasked=TRUE} but there are no masked features.}

  The elements of the cell values are ordered according to argument 
  \code{indices}.  The lengths of the cell-value elements equals the
  number of cells read.
  
  Which of the above elements that are returned are controlled by the 
  \code{readNnn} arguments.  If \code{FALSE}, the corresponding element 
  above is \code{NULL}, e.g. if \code{readStdvs=FALSE} then 
  \code{stdvs} is \code{NULL}.
}

  
\section{Outliers and masked cells}{
  The Affymetrix image analysis software flags cells as outliers and masked.
  This method does not return these flags, but instead vectors of cell 
  indices listing which cells \emph{of the queried cells} are outliers and
  masked, respectively.
  The current community view seems to be that this should be done based on
  statistical modelling of the actual probe intensities and should be based
  on the choice of preprocessing algorithm.  Most algorithms are only using
  the intensities from the CEL file.
}

\section{Memory usage}{
  The Fusion SDK allocates memory for the entire
  CEL file, when the file is accessed (but does not actually read the
  file into memory). Using the \code{indices} argument will therefore
  only affect the memory use of the final object (as well as speed), not
  the memory allocated in the C function used to parse the file. This
  should be a minor problem however.
}

\section{Troubleshooting}{
  It is considered a bug if the file contains information not accessible 
  by this function, please report it.
}


\examples{
  for (zzz in 0) {  # Only so that 'break' can be used

  # Scan current directory for CEL files
  celFiles <- list.files(pattern="[.](c|C)(e|E)(l|L)$")
  if (length(celFiles) == 0)
    break;

  celFile <- celFiles[1]

  # Read a subset of cells
  idxs <- c(1:5, 1250:1500, 450:440)
  cel <- readCel(celFile, indices=idxs, readOutliers=TRUE)
  str(cel)

  # Clean up
  rm(celFiles, celFile, cel)

  } # for (zzz in 0)
}

\author{James Bullard, \email{bullard@stat.berkeley.edu} and Kasper
  Daniel Hansen, \email{khansen@stat.berkeley.edu}} 

\seealso{
  \code{\link{readCelHeader}()} for a description of the header output.
  Often a user only wants to read the intensities, look at
  \code{\link{readCelIntensities}()} for a function specialized for 
  that use. 
}

\keyword{file}
\keyword{IO}

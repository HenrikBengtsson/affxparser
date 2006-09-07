%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  updateCelUnits.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{updateCelUnits}
\alias{updateCelUnits}


 \title{Updates a CEL file unit by unit}

 \usage{updateCelUnits(filename, cdf=NULL, data, ..., verbose=0)}

 \description{
   Updates a CEL file unit by unit.\cr

   \emph{Please note that, contrary to \code{\link{readCelUnits}}(), this method
   can only update a single CEL file at the time.}
 }

 \arguments{
   \item{filename}{The filename of the CEL file.}
   \item{cdf}{A (optional) CDF \code{\link[base]{list}} structure either with
     field \code{indices} or fields \code{x} and \code{y}.
     If \code{\link[base]{NULL}}, the unit names (and from there the cell indices) are
     inferred from the names of the elements in \code{data}.
   }
   \item{data}{A \code{\link[base]{list}} structure in a format similar to what is returned
      by \code{\link{readCelUnits}}() for \emph{a single CEL file only}.}
   \item{...}{Optional arguments passed to \code{\link{readCdfCellIndices}}(),
      which is called if \code{cdf} is not given.}
   \item{verbose}{An \code{\link[base]{integer}} specifying how much verbose details are
     outputted.}
 }

 \value{
   Returns what \code{\link{updateCel}}() returns.
 }

 \section{Working with re-arranged CDF structures}{
   Note that if the \code{cdf} structure is specified the CDF file is
   \emph{not} queried, but all information about cell x and y locations,
   that is, cell indices is expected to be in this structure.  This can
   be very useful when one work with a cdf structure that originates
   from the underlying CDF file, but has been restructured for instance
   through the \code{\link{applyCdfGroups}}() method, and \code{data}
   correspondingly.  This update method knows how to update such
   structures too.
 }

 \examples{
for (zzz in 0) {

# CEL file to use in this demostration
pathname <- "tmp.cel"
if (!file.exists(pathname)) {
  # Scan current directory for a CEL file
  file <- findFiles(pattern="[.](c|C)(e|E)(l|L)$")
  if (is.null(file))
    break

  # Work on a copy of the CEL file
  cat("Copying CEL file: ", file, "\n", sep="");
  if (!file.copy(file, pathname))
    stop("Failed to copy CEL file: ", file);
  rm(file)
}


# Check for the CDF file
hdr <- readCelHeader(pathname)
cdfFile <- findCdf(hdr$chiptype)
if (is.null(cdfFile))
  break

hdr <- readCdfHeader(cdfFile)
nbrOfUnits <- hdr$probesets

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Example: Read and re-write the same data
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
units <- c(101, 51)
data1 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
cat("Original data:\n")
str(data1)
updateCelUnits(pathname, data=data1)
data2 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
cat("Updated data:\n")
str(data2)
stopifnot(identical(data1, data2))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Example: Random read and re-write "stress test"
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
for (kk in 1:100) {
  nunits <- sample(min(1000,nbrOfUnits), size=1)
  units <- sample(nbrOfUnits, size=nunits)
  cat(sprintf("\%02d. Selected \%d random units: reading", kk, nunits));
  t <- system.time({
    data1 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
  }, gcFirst=TRUE)[3]
  cat(sprintf(" [\%.2fs=\%.2fs/unit], updating", t, t/nunits))
  t <- system.time({
    updateCelUnits(pathname, data=data1)
  }, gcFirst=TRUE)[3]
  cat(sprintf(" [\%.2fs=\%.2fs/unit], validating", t, t/nunits))
  data2 <- readCelUnits(pathname, units=units, readStdvs=TRUE)
  stopifnot(identical(data1, data2))
  cat(". done\n")
}

} # for (zzz in 0)
}

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}

 \seealso{
   Internally, \code{\link{updateCel}}() is used.
 }



\keyword{file}
\keyword{IO}

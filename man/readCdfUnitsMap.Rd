%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  
% 
% on Wed Mar 29 20:22:52 2006.
% 
% Generator was the Rdoc class, which is part of the R.oo package written
% by Henrik Bengtsson, 2001-2006.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCdfUnitsMap}
\alias{readCdfUnitsMap}


 \title{Generates an Affymetrix cell-index map from a CDF file}

 \usage{readCdfUnitsMap(filename, units=NULL, ..., writeMap=FALSE, verbose=FALSE)}

 \description{
   Generates an Affymetrix cell-index map from a CDF file.

   The purpose of this method is to provide a re-ordering of cell elements
   such that cells in units (probesets) can be stored in contigous blocks.
   When reading cell elements unit by unit, minimal file re-position is
   required resulting in a faster reading.

   Note: At the moment does this package not provide methods to
   write/reorder CEL files.  In the meanwhile, you have to write
   and reread using your own file format.  That's not too hard using
   \code{writeBin()} and \code{\link[base]{readBin}}().
 }

 \arguments{
   \item{filename}{The pathname of the CDF file.}
   \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices specifying which units
     to listed first.  All other units are added in order at the end.
     If \code{\link[base]{NULL}}, units are in order.}
   \item{...}{Additional arguments passed to \code{\link{readCdfUnits}}().}
   \item{writeMap}{If \code{\link[base:logical]{TRUE}}, the write map, that is, the map for writing
     cell elements is returned, otherwise the read map is returned.}
   \item{verbose}{Either a \code{\link[base]{logical}}, a \code{\link[base]{numeric}}, or a \code{\link[R.utils]{Verbose}}
     object specifying how much verbose/debug information is written to
     standard output. If a Verbose object, how detailed the information is
     is specified by the threshold level of the object. If a numeric, the
     value is used to set the threshold of a new Verbose object. If \code{\link[base:logical]{TRUE}},
     the threshold is set to -1 (minimal). If \code{\link[base:logical]{FALSE}}, no output is written
     (and neither is the \pkg{R.utils} package required).}
 }

 \value{
   An \code{\link[base]{integer}} \code{\link[base]{vector}} of length \eqn{n} with unique elements in
   \eqn{[1,n]}, where \eqn{n} is the number of cells on the array.
 }

 \details{
   If \code{writeMap} is \code{\link[base:logical]{TRUE}}, the cell-index map for writing cell
   elements is returned.  If \code{\link[base:logical]{FALSE}}, the map for reading cell elements is
   returned.  The relationship between the read map and the write map is
   \code{readMap <- order(writeMap)} and \code{writeMap <- order(readMap)}.
   Yes, \code{map <- order(order(map))}.
 }

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}

 \examples{
  
for (zzz in 0) {

# Find any CDF file
cdfFile <- findCdf()
if (is.null(cdfFile))
  break;

# Create a cell-index map (for reading)
readMap <- readCdfUnitsMap(cdfFile)

# Inverse map to be used to write cell elements such that, if they
# later are read unit by unit, they are read in contiguous blocks.
writeMap <- match(1:length(readMap), readMap)  # faster than order()

# Validate the two maps
stopifnot(identical(readMap[writeMap], 1:length(readMap)))


cat("Summary of the \"randomness\" of the cell indices:\n")
moves <- diff(readMap) - 1
cat(sprintf("Number of unnecessary file re-positioning: \%d (\%.1f\%\%)\n",
                   sum(moves != 0), 100*sum(moves != 0)/length(moves)))
cat(sprintf("Extra positioning in Gb: \%.1f\n", sum(abs(moves))/1024^3))

smallMoves <- moves[abs(moves) <= 25];
largeMoves <- moves[abs(moves)  > 25];
layout(matrix(1:2))
main <- "Non-signed file moves required in unorded file"
hist(smallMoves, nclass=51, main=main, xlab="moves <=25 bytes")
hist(largeMoves, nclass=101, main="", xlab="moves >25 bytes")

# Clean up
rm(cdfFile, readMap, writeMap, moves, smallMoves, largeMoves, main)

} # for (zzz in 0)


  
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Function to read Affymetrix probeset annotations
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
readAffymetrixProbesetAnnotation <- function(pathname, ...) {
  # Get headers
  header <- scan(pathname, what="character", sep=",", quote="\"",
                                                      quiet=TRUE, nlines=1);

  # Read only a subset of columns (unique to this example)
  cols <- c("Probe Set ID"="probeSet",
            "Chromosome"="chromosome",
            "Physical Position"="physicalPosition",
            "dbSNP RS ID"="dbSnpId");

  colClasses <- rep("NULL", length(header));
  colClasses[header \%in\% names(cols)] <- "character";

  # Read the data (this is what takes time)
  df <- read.table(pathname, colClasses=colClasses, header=TRUE, sep=",",
         quote="\"", na.strings="---", strip.white=TRUE, check.names=FALSE,
                 blank.lines.skip=FALSE, fill=FALSE, comment.char="", ...);

  # Re-order columns
  df <- df[,match(names(cols),colnames(df))];
  colnames(df) <- cols;

  # Use "Probe Set ID" as rownames. Note that if we use 'row.names=1'
  # or similar something goes wrong. /HB 2006-03-06
  rownames(df) <- df[[1]];
  df <- df[,-1]; 

  # Change types of columns
  df[[1]] <- factor(df[[1]], levels=c(1:22,"X","Y",NA), ordered=TRUE);
  df[[2]] <- as.integer(df[[2]]);

  df;
} # readAffymetrixProbesetAnnotation()



# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Main
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
for (zzz in 0) {

# Chip to be remapped
chipType <- "Mapping50K_Xba240"

annoFile <- paste(chipType, "_annot.csv", sep="")
cdfFile <- findCdf(chipType)
if (is.null(cdfFile) || !file.exists(annoFile))
  break;

# Read SNP location details
snpInfo <- readAffymetrixProbesetAnnotation(annoFile)

# Order by chromsome and then physical position
o <- order(snpInfo[[1]], snpInfo[[2]])
snpInfo <- snpInfo[o,]
rm(o)

# Read unit names in CDF file
unitNames <- readCdfUnitNames(cdfFile)

# The CDF unit indices sorted by chromsomal position
units <- match(rownames(snpInfo), unitNames)

# ...and cell indices in the same order
readMap <- readCdfUnitsMap(cdfFile, units=units)

# Inverse map to be used to write cell elements such that, if they
# later are read unit by unit, they are read in contiguous blocks.
writeMap <- match(1:length(readMap), readMap)  # faster than order()

# Clean up
rm(chipType, annoFile, cdfFile, snpInfo, unitNames, units, readMap, writeMap)

} # for (zzz in 0)

 }

 \seealso{
   \code{\link{readCel}}() and \code{\link{readCelUnits}}().
 }



\keyword{file}
\keyword{IO}

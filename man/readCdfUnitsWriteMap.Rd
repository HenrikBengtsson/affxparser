%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdfUnitsWriteMap.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{readCdfUnitsWriteMap}
\alias{readCdfUnitsWriteMap}


\title{Generates an Affymetrix cell-index write map from a CDF file}

\usage{
readCdfUnitsWriteMap(filename, units=NULL, ..., verbose=FALSE)
}

\description{
  Generates an Affymetrix cell-index write map from a CDF file.

  The purpose of this method is to provide a re-ordering of cell elements
  such that cells in units (probesets) can be stored in contigous blocks.
  When reading cell elements unit by unit, minimal file re-position is
  required resulting in a faster reading.

  Note: At the moment does this package not provide methods to
  write/reorder CEL files.  In the meanwhile, you have to write
  and re-read using your own file format.  That's not too hard using
  \code{writeBin()} and \code{\link[base]{readBin}}().
}

\arguments{
  \item{filename}{The pathname of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices specifying which units
    to listed first.  All other units are added in order at the end.
    If \code{\link[base]{NULL}}, units are in order.}
  \item{...}{Additional arguments passed to \code{\link{readCdfUnits}}().}
  \item{verbose}{Either a \code{\link[base]{logical}}, a \code{\link[base]{numeric}}, or a \code{\link[R.utils]{Verbose}}
    object specifying how much verbose/debug information is written to
    standard output. If a Verbose object, how detailed the information is
    is specified by the threshold level of the object. If a numeric, the
    value is used to set the threshold of a new Verbose object. If \code{\link[base:logical]{TRUE}},
    the threshold is set to -1 (minimal). If \code{\link[base:logical]{FALSE}}, no output is written
    (and neither is the \pkg{R.utils} package required).}
}

\value{
  A \code{\link[base]{integer}} \code{\link[base]{vector}} which is a \emph{write} map.
}

\author{Henrik Bengtsson}

\examples{
  ##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Find any CDF file
cdfFile <- findCdf()

# Create a cell-index map (for writing)
writeMap <- readCdfUnitsWriteMap(cdfFile)

# Inverse map to be used to read cell elements such that, when read
# read unit by unit, they are read much faster.
readMap <- invertMap(writeMap)

# Validate the two maps
stopifnot(identical(readMap[writeMap], 1:length(readMap)))


cat("Summary of the \"randomness\" of the cell indices:\n")
moves <- diff(readMap) - 1
cat(sprintf("Number of unnecessary file re-positioning: \%d (\%.1f\%\%)\n",
                   sum(moves != 0), 100*sum(moves != 0)/length(moves)))
cat(sprintf("Extra positioning: \%.1fGb\n", sum(abs(moves))/1024^3))

smallMoves <- moves[abs(moves) <= 25];
largeMoves <- moves[abs(moves)  > 25];
layout(matrix(1:2))
main <- "Non-signed file moves required in unorded file"
hist(smallMoves, nclass=51, main=main, xlab="moves <=25 bytes")
hist(largeMoves, nclass=101, main="", xlab="moves >25 bytes")

# Clean up
layout(1)
rm(cdfFile, readMap, writeMap, moves, smallMoves, largeMoves, main)

##############################################################
}                                                     # STOP #
##############################################################



  ##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

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
for (zz in 1) {
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
writeMap <- readCdfUnitsWriteMap(cdfFile, units=units)

# Inverse map to be used to write cell elements such that, if they
# later are read unit by unit, they are read in contiguous blocks.
readMap <- invertMap(writeMap)

# Clean up
rm(chipType, annoFile, cdfFile, snpInfo, unitNames, units, readMap, writeMap)

} # for (zz in 1)
##############################################################
}                                                     # STOP #
##############################################################

}

\seealso{
  To invert maps, see \code{\link{invertMap}}().
  \code{\link{readCel}}() and \code{\link{readCelUnits}}().
}




\keyword{file}
\keyword{IO}
\keyword{internal}

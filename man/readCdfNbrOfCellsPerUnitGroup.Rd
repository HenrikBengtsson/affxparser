%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdfNbrOfCellsPerUnitGroup.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{readCdfNbrOfCellsPerUnitGroup}
\alias{readCdfNbrOfCellsPerUnitGroup}


 \title{Gets the number of cells (probes) that each group of each unit in a CDF file}

 \usage{readCdfNbrOfCellsPerUnitGroup(filename, units=NULL, verbose=0)}

 \description{
   Gets the number of cells (probes) that each group of each unit in a CDF file.
 }

 \arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices specifying which units
     to be read.  If \code{\link[base]{NULL}}, all units are read.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
     file is parsed quietly.  The higher numbers, the more details.}
 }

 \value{
  A named \code{\link[base]{list}} of named \code{\link[base]{integer}} vectors.  The name of the list elements
  are unit names and the names of the integer vector are group names.
 }

 \examples{
for (zzz in 0) {

cdfFile <- findCdf("Mapping50K_Xba240")
if (is.null(cdfFile))
  break

groups <- readCdfNbrOfCellsPerUnitGroup(cdfFile)

# Number of units read
print(length(groups))
#   59015

# Details on two units
print(groups[56:57])
# $`SNP_A-1650338`
#  C  G  C  G
#  8  8 12 12
#
# $`SNP_A-1716667`
#  A  G  A  G
# 10 10 10 10

# Number of groups with different number of cells
print(table(unlist(groups)))
#      6     8    10    12    14    16    60
#  16348 59462 84344 59462 16348    20     4


# Number of cells per unit
nbrOfCellsPerUnit <- unlist(lapply(groups, FUN=sum))
print(table(nbrOfCellsPerUnit))
#   16    40    60
#   20 58991     4


# Number of groups per unit
nbrOfGroupsPerUnit <- unlist(lapply(groups, FUN=length))

# Details on a few units
print(nbrOfGroupsPerUnit[20:30])
# AFFX-barcodeP AFFX-barcodeQ AFFX-barcodeR AFFX-barcodeS AFFX-barcodeT
#             1             1             1             1             1
#   AFFX-601964   AFFX-656757   AFFX-721431   AFFX-737848  AFFX-1329481
#             4             4             4             4             4
#  AFFX-1375402
#             4

# Number of units for each unique number of groups
print(table(nbrOfGroupsPerUnit))
#   1     4
#  24 58991

x <- list()
for (size in unique(nbrOfGroupsPerUnit)) {
  subset <- groups[nbrOfGroupsPerUnit==size]
  t <- matrix(unlist(subset), nrow=size)
  colnames(t) <- names(subset)
  x[[as.character(size)]] <- t
  rm(subset, t)
}

# Check if there are any quartet units where the number
# of cells in Group 1 & 2 or Group 3 & 4 does not have
# the same number of cells.
# Group 1 & 2
print(sum(x[["4"]][1,]-x[["4"]][2,] != 0))
# 0

# Group 3 & 4
print(sum(x[["4"]][3,]-x[["4"]][4,] != 0))
# 0

# Clean up
rm(cdfFile, groups, nbrOfCellsPerUnit, nbrOfGroupsPerUnit, x)
} # for (zzz in 0)
}
 \author{Henrik Bengtsson \url{http://www.braju.com/R/}}




\keyword{file}
\keyword{IO}
\keyword{internal}

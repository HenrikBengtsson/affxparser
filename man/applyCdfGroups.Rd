%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  applyCdfGroups.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{applyCdfGroups}
\alias{applyCdfGroups}

\alias{applyCdfBlocks}

\title{Applies a function over the groups in a CDF structure}

\description{
 Applies a function over the groups in a CDF structure.
}

\usage{applyCdfGroups(cdf, fcn, ...)}

\arguments{
 \item{cdf}{A CDF \code{\link[base]{list}} structure.}
 \item{fcn}{A \code{\link[base]{function}} that takes a \code{\link[base]{list}} structure of group elements
  and returns an updated \code{\link[base]{list}} of groups.}
 \item{...}{Arguments passed to the \code{fcn} function.}
}

\value{
 Returns an updated CDF \code{\link[base]{list}} structure.
}

\section{Pre-defined restructuring functions}{
 \itemize{
  \item{Generic:}{
  \itemize{
   \item \code{\link{cdfGetFields}}() - Gets a subset of groups fields in a CDF
     structure.
   \item \code{\link{cdfGetGroups}}() - Gets a subset of groups in a CDF structure.
   \item \code{\link{cdfOrderBy}}() - Orders the fields according to the value of
     another field in the same CDF group.
   \item \code{\link{cdfOrderColumnsBy}}() - Orders the columns of fields according
     to the values in a certain row of another field in the same CDF group.
  }}
  \item{Designed for SNP arrays:}{
  \itemize{
   \item \code{\link{cdfAddBaseMmCounts}}() - Adds the number of allele A and
     allele B mismatching nucleotides of the probes in a CDF structure.
   \item \code{\link{cdfAddProbeOffsets}}() - Adds probe offsets to the groups in
     a CDF structure.
   \item \code{\link{cdfGtypeCelToPQ}}() - Function to immitate Affymetrix'
     \code{gtype_cel_to_pq} software.
   \item \code{\link{cdfMergeAlleles}}() - Function to join CDF allele A and
     allele B groups strand by strand.
   \item \code{\link{cdfMergeStrands}}() - Function to join CDF groups with the
     same names.
  }}
 }

 We appreciate contributions.
}

\examples{

for (zzz in 0) {

# Find the CDF file for the Mapping 100K Xba array
chipType <- "Mapping50K_Xba240";
cdfFile <- findCdf(chipType)
if (is.null(cdfFile))
  break;

# Probeset SNP_A-1658298 (dbSNP rs7034018) has equal number of
# sense and anti-sense probes, but for the latter group the
# SNP "interrogating" probe pair has been excluded in
# Affymetrix' probe selection process when designing the
# Mapping 100K Xba array.

# Identify the unit index from the unit name
probeset <- "SNP_A-1658298"
unit <- which(readCdfUnitNames(cdfFile) == probeset)
rm(probeset)

# Read the CDF file
cdf0 <- readCdfUnits(cdfFile, units=unit, stratifyBy="pmmm")
cat("Default CDF structure:\n")
print(cdf0)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Tabulate the information in each group
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdf <- readCdfUnits(cdfFile, units=unit)
cdf <- applyCdfGroups(cdf, lapply, as.data.frame)
print(cdf)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Infer the (true or the relative) offset for probe quartets.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdf <- applyCdfGroups(cdf0, cdfAddProbeOffsets)
cat("Probe offsets:\n")
print(cdf)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Identify the number of nucleotides that mismatch the
# allele A and the allele B sequences, respectively.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdf <- applyCdfGroups(cdf, cdfAddBaseMmCounts)
cat("Allele A & B target sequence mismatch counts:\n")
print(cdf)



# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Combine the signals from  the sense and the anti-sense
# strands in a SNP CEL files.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# First, join the strands in the CDF structure.
cdf <- applyCdfGroups(cdf, cdfMergeStrands)
cat("Joined CDF structure:\n")
print(cdf)

# Then, read the CEL file using this new structure
# celFile <- "NA06985_Hind_B5_3005533.CEL"
# cel <- readCelUnits(celFile, cdf=cdf)
# cat("Joined CEL intensities:\n")
# print(cel)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Rearrange values of group fields into quartets.  This
# requires that the values are already arranged as PMs and MMs.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdf <- applyCdfGroups(cdf0, cdfMergeAlleles)
cat("Probe quartets:\n")
print(cdf)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Get the x and y cell locations
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
x <- unlist(applyCdfGroups(cdf, cdfGetFields, "x"), use.names=FALSE)
y <- unlist(applyCdfGroups(cdf, cdfGetFields, "y"), use.names=FALSE)

# Clean up
rm(chipType, cdfFile, unit, cdf0, cdf, x, y)
} # for (zzz in 0)
rm(zzz)

}

\author{Henrik Bengtsson \url{http://www.braju.com/R/}}


\keyword{programming}

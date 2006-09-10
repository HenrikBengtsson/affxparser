%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  applyCdfBlocks.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{applyCdfBlocks}
\alias{applyCdfBlocks}


\title{Applies a function over the blocks in a CDF structure}

\description{
 Applies a function over the blocks in a CDF structure.
}

\usage{applyCdfBlocks(cdf, fcn, ..., .key="blocks")}

\arguments{
 \item{cdf}{A CDF \code{\link[base]{list}} structure.}
 \item{fcn}{A \code{\link[base]{function}} that takes a \code{\link[base]{list}} structure of block elements
  and returns an updated \code{\link[base]{list}} of blocks.}
 \item{...}{Arguments passed to the \code{fcn} function.}
 \item{.key}{The name of the unit element containing the blocks.  This
  used for backward compatibility when this field was also called
  \code{groups}.}
}

\value{
 Returns an updated CDF \code{\link[base]{list}} structure.
}

\section{Pre-defined restructuring functions}{
 \itemize{
  \item{Generic:}{
  \itemize{
   \item \code{\link{cdfGetFields}}() - Gets a subset of blocks fields in a CDF
     structure.
   \item \code{\link{cdfGetBlocks}} - Gets a subset of blocks in a CDF structure.
   \item \code{\link{cdfOrderBy}}() - Orders the fields according to the value of
     another field in the same CDF block.
   \item \code{\link{cdfOrderColumnsBy}}() - Orders the columns of fields according
     to the values in a certain row of another field in the same CDF block.
  }}
  \item{Designed for SNP arrays:}{
  \itemize{
   \item \code{\link{cdfAddBaseMmCounts}}() - Adds the number of allele A and
     allele B mismatching nucleotides of the probes in a CDF structure.
   \item \code{\link{cdfAddProbeOffsets}}() - Adds probe offsets to the blocks in
     a CDF structure.
   \item \code{\link{cdfGtypeCelToPQ}}() - Function to immitate Affymetrix'
     \code{gtype_cel_to_pq} software.
   \item \code{\link{cdfMergeAlleles}}() - Function to join CDF allele A and
     allele B blocks strand by strand.
   \item \code{\link{cdfMergeStrands}}() - Function to join CDF blocks with the
     same names.
  }}
 }

 We appreciate contributions.
}

\author{Henrik Bengtsson (\url{http://www.braju.com/R/})}


\keyword{programming}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdfGroupNames.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{readCdfGroupNames}
\alias{readCdfGroupNames}


\title{Reads group names for a set of units (probesets) in an Affymetrix CDF file}

\usage{
readCdfGroupNames(filename, units=NULL, truncateGroupNames=TRUE, verbose=0)
}

\description{
  Reads group names for a set of units (probesets) in an Affymetrix CDF file.

  This is for instance useful for SNP arrays where the nucleotides used
  for the A and B alleles are the same as the group names.
}

\arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices specifying which
     units to be read.  If \code{\link[base]{NULL}}, all units are read.}
   \item{truncateGroupNames}{A \code{\link[base]{logical}} variable indicating whether unit
     names should be stripped from the beginning of group names.}
   \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
     file is parsed quietly.  The higher numbers, the more details.}
}

\value{
   A named \code{\link[base]{list}} structure where the names of the elements are the names
   of the units read.  Each element is a \code{\link[base]{character}} \code{\link[base]{vector}} with group
   names for the corrsponding unit.
 }

\author{Henrik Bengtsson}

 \seealso{
   \code{\link{readCdfUnits}}().
 }





\keyword{file}
\keyword{IO}

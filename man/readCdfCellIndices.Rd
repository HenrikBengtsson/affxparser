%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  readCdfCellIndices.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{readCdfCellIndices}
\alias{readCdfCellIndices}


\title{Reads (one-based) cell indices of units (probesets) in an Affymetrix CDF file}

\usage{
readCdfCellIndices(filename, units=NULL, stratifyBy=c("nothing", "pmmm", "pm", "mm"),
  verbose=0)
}

\description{
  Reads (one-based) cell indices of units (probesets) in an Affymetrix CDF file.
}

\arguments{
  \item{filename}{The filename of the CDF file.}
  \item{units}{An \code{\link[base]{integer}} \code{\link[base]{vector}} of unit indices
    specifying which units to be read.  If \code{\link[base]{NULL}}, all units are read.}
  \item{stratifyBy}{A \code{\link[base]{character}} string specifying which and how
    elements in group fields are returned.
    If \code{"nothing"}, elements are returned as is, i.e. as \code{\link[base]{vector}}s.
    If \code{"pm"}/\code{"mm"}, only elements corresponding to
    perfect-match (PM) / mismatch (MM) probes are returned (as \code{\link[base]{vector}}s).
    If \code{"pmmm"}, elements are returned as a matrix where the
    first row holds elements corresponding to PM probes and the second
    corresponding to MM probes.  Note that in this case, it is assumed
    that there are equal number of PMs and MMs; if not, an error is
    generated.
    Moreover, the PMs and MMs may not even be paired, i.e. there is no
    guarantee that the two elements in a column corresponds to a
    PM-MM pair.}
  \item{verbose}{An \code{\link[base]{integer}} specifying the verbose level. If 0, the
    file is parsed quietly.  The higher numbers, the more details.}
}

\value{
  A named \code{\link[base]{list}} where the names corresponds to the names
  of the units read.  Each unit element of the list is in turn a
  \code{\link[base]{list}} structure with one element \code{groups} which in turn
  is a \code{\link[base]{list}}.  Each group element in \code{groups} is a \code{\link[base]{list}}
  with a single field named \code{indices}.  Thus, the structure is
  \preformatted{
    cdf
     +- unit #1
     |   +- "groups"
     |       +- group #1
     |       |   +- "indices"
     |       |  group #2
     |       |   +- "indices"
     |       .
     |       +- group #K
     |           +- "indices"
     +- unit #2
     .
     +- unit #J
  }

  This is structure is compatible with what \code{\link{readCdfUnits}}() returns.

  Note that these indices are \emph{one-based}.
}

\section{Cell indices are one-based}{
  Note that in \pkg{affxparser} all \emph{cell indices} are by
  convention \emph{one-based}, which is more convenient to work
  with in \R.  For more details on one-based indices, see
  \code{\link{2. Cell coordinates and cell indices}}.
}

\author{Henrik Bengtsson}

 \seealso{
   \code{\link{readCdfUnits}}().
 }



\keyword{file}
\keyword{IO}

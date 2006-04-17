%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  901.CellCoordinatesAndIndices.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{cellxy}
\alias{cellxy}
\title{Cell coordinates and cell indices}

 \description{
   This part describes how Affymetrix \emph{cells}, also known as
   \emph{probes} or \emph{features}, are address.
 }

 \section{Cell coordinates}{
   In Affymetrix data files, cells are uniquely identified by there
   \emph{cell coordinates}, i.e. \eqn{(x,y)}.  For an array with
   \eqn{N*K} cells in \eqn{N} rows and \eqn{K} columns, the \eqn{x}
   coordinate is an integer in \eqn{[0,K-1]}, and the \eqn{y} coordinate
   is an integer in \eqn{[0,N-1]}.  The cell in the upper-left corner has
   coordinate \eqn{(x,y)=(0,0)} and the one in the lower-right corner
   \eqn{(x,y)=(K-1,N-1)}.
 }

 \section{Cell indices and cell-index offsets}{
   To simplify addressing of cells, a coordinate-to-index function is
   used so that each cell can be addressed using a single integer instead
   (of two).  Affymetrix defines the \emph{cell index}, \eqn{i}, of
   cell \eqn{(x,y)} as
   \deqn{
     i = K*y + x + 1,
   }
   where one is added to give indices in \eqn{[1,N*K]}.
   Continuing, the above definition means that cells are ordered
   row by row, that is from left to right and from top to bottom,
   starting at the upper-left corner.
   For example, with a chip layout \eqn{(N,K)=(1600,1600)} the cell at
   \eqn{(x,y)=(0,0)} has index i=1, and the cell at \eqn{(x,y)=(1599,1599)}
   has index \eqn{i=2600000}.

   Given the cell index \eqn{i}, the coordinate \eqn{(x,y)} can be
   calculated as
   \deqn{
     y = mod(i-1, K)
   }
   \deqn{
     x = (i-1)-K*y.
   }
   Continuing the above example, the coordinate for cell \eqn{i=1} is
   thus \eqn{(x,y)=(0,0)} and for cell \eqn{i=2600000} it is
   \eqn{(x,y)=(1599,1599)}.

   Although not needed to use the methods in this package, to get the
   cell indices for the cell coordinates or vice versa, see
   \code{\link[affy]{xy2indices}} and \code{indices2xy()} in the
   \pkg{affy} package.
 }

 \author{Henrik Bengtsson (\url{http://www.braju.com/R/})}
\keyword{documentation}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  902.CellCoordinatesAndIndices.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{2. Cell coordinates and cell indices}
\alias{2. Cell coordinates and cell indices}
\title{2. Cell coordinates and cell indices}


 \description{
   This part describes how Affymetrix \emph{cells}, also known as
   \emph{probes} or \emph{features}, are addressed.
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
   has index \eqn{i=2560000}.
   A cell at \eqn{(x,y)=(1498,3)} has index \eqn{i=6299}.

   Given the cell index \eqn{i}, the coordinate \eqn{(x,y)} can be
   calculated as
   \deqn{
     y = floor((i-1)/K)
   }
   \deqn{
     x = (i-1)-K*y.
   }
   Continuing the above example, the coordinate for cell \eqn{i=1} is
   be found to be \eqn{(x,y)=(0,0)}, for cell \eqn{i=2560000} it is
   \eqn{(x,y)=(1599,1599)}, for cell \eqn{i=6299} is it
   \eqn{(x,y)=(1498,3)}.
 }

 \section{Converting between cell indices and (x,y) coordinates in R}{
   Although not needed to use the methods in this package, to get the
   cell indices for the cell coordinates or vice versa, see
   \code{\link[affy:xy2indices]{xy2indices}()} and \code{indices2xy()}
   in the \pkg{affy} package.
 }

 \section{Note on the zero-based "index" field of Affymetrix CDF files}{
   An Affymetrix CDF file provides information on which cells should be
   grouped together.  To identify these groups of cells, the cells
   are specified by their (x,y) coordinates, which are stored as
   zero-based coordinates in the CDF file.

   All methods of the \pkg{affxparser} package make use of these
   (x,y) coordinates, and some methods make it possible to read
   them as well.  However, it is much more common that the methods
   return cell indices \emph{calculated} from the (x,y) coordinates
   as explained above.

   In order to conveniently work with cell indices in \R, the
   convention in \emph{affxparser} is to use \emph{one-based}
   indices.
   Hence the addition (and subtraction) of 1:s in the above equations.
   This is all taken care of by \pkg{affxparser}.

   Note that, in addition to (x,y) coordinates, a CDF file also contains
   a one-based "index" for each cell.  This "index" is redundant to
   the (x,y) coordinate and can be calculated analogously to the
   above \emph{cell index} while leaving out the addition (subtration)
   of 1:s.
   Importantly, since this "index" is redundant (and exists only in
   CDF files), we have decided to treat this field as an internal field.
   Methods of \pkg{affxparser} do neither provide access to nor make
   use of this internal field.
 }

 \author{Henrik Bengtsson}
\keyword{documentation}

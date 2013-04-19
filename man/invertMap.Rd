%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  invertMap.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 \name{invertMap}
\alias{invertMap}


 \title{Inverts a read or a write map}

 \usage{
invertMap(map, ...)
}

 \description{
   Inverts a read or a write map.
 }

 \arguments{
   \item{map}{An \code{\link[base]{integer}} \code{\link[base]{vector}}.}
   \item{...}{Not used.}
 }

 \value{
   Returns an \code{\link[base]{integer}} \code{\link[base]{vector}}.
 }

 \details{
   An map is defined to be a \code{\link[base]{vector}} of \emph{n} with unique finite
   values in \eqn{[1,n]}.  Finding the inverse of a map is the same as
   finding the rank of each element, cf. \code{\link[base]{order}}().  However,
   this method is much faster, because it utilizes the fact that all
   values are unique and in \eqn{[1,n]}.  Moreover, for any map it holds
   that taking the inverse twice will result in the same map.
 }

 \examples{
set.seed(1)

# Simulate a read map for a chip with 1.2 million cells
nbrOfCells <- 1200000
readMap <- sample(nbrOfCells)

# Get the corresponding write map
writeMap <- invertMap(readMap)

# A map inverted twice should be equal itself
stopifnot(identical(invertMap(writeMap), readMap))

# Another example illustrating that the write map is the
# inverse of the read map
idx <- sample(nbrOfCells, size=1000)
stopifnot(identical(writeMap[readMap[idx]], idx))

# invertMap() is much faster than order()
t1 <- system.time(invertMap(readMap))[3]
cat(sprintf("invertMap()  : \%5.2fs [ 1.00x]\n", t1))

t2 <- system.time(writeMap2 <- sort.list(readMap, na.last=NA, method="quick"))[3]
cat(sprintf("'quick sort' : \%5.2fs [\%5.2fx]\n", t2, t2/t1))
stopifnot(identical(writeMap, writeMap2))

t3 <- system.time(writeMap2 <- order(readMap))[3]
cat(sprintf("order()      : \%5.2fs [\%5.2fx]\n", t3, t3/t1))
stopifnot(identical(writeMap, writeMap2))

# Clean up
rm(nbrOfCells, idx, readMap, writeMap, writeMap2)

}

 \author{Henrik Bengtsson}

 \seealso{
   To generate an optimized write map for a CDF file, see
   \code{\link{readCdfUnitsWriteMap}}().
 }




\keyword{file}
\keyword{IO}
\keyword{internal}

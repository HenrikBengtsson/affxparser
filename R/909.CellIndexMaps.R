#########################################################################/**
# @RdocDocumentation "9. Advanced - Cell-index maps for reading and writing"
#
# \description{
#   This part defines read and write maps that can be used to remap 
#   cell indices before reading and writing data from and to file,
#   respectively.
#
#   This package provides methods to create read and write (cell-index) 
#   maps from Affymetrix CDF files.  These can be used to store the cell
#   data in an optimal order so that when data is read it is read in
#   contiguous blocks, which is faster.
#   Currently, this package does not provide methods for writing 
#   Affymetrix data files such as CEL files.  However, other package 
#   might do, which is why the methods here support read maps, e.g.
#   @see "readCelUnits".
#
#   For more details how cell indices are defined, see 
#   @see "2. Cell coordinates and cell indices".
# }
#
# \section{Motivation}{
#   When reading data from file, it is faster to read the data in
#   the order that it is stored compared with, say, in a random order.
#   The main reason for this is that the read arm of the harddrive
#   has to move more if data is not read consecutively.  Same applies
#   when writing data to file.  The read and write cache of the file
#   system may compensate a bit for this, but not completely.
#
#   In Affymetrix CEL files, cell data is stored in order of cell indices.
#   Moreover, (except for a few early chip types) Affymetrix randomizes
#   the locations of the cells such that cells in the same unit (probeset)
#   are scattered across the array.  
#   Thus, when reading CEL data arranged by units using for instance 
#   @see "readCelUnits", the order of the cells requested is both random
#   and scattered.  
#   
#   Since CEL data is often queried unit by unit (except for some
#   probe-level normalization methods), one can improve the speed of
#   reading data by saving data such that cells in the same unit are
#   stored together.  A \emph{write map} is used to remap cell indices
#   to file indices.  When later reading that data back, a 
#   \emph{read map} is used to remap file indices to cell indices.
#   Read and write maps are described next.
# }
#
# \section{Definition of read and write maps}{
#   Consider cell indices \eqn{i=1, 2, ..., N*K} and file indices 
#   \eqn{j=1, 2, ..., N*K}.
#   A \emph{read map} is then a \emph{bijective} (one-to-one) function
#   \eqn{h()} such that
#   \deqn{
#     i = h(j),
#   }
#   and the corresponding \emph{write map} is the inverse function
#   \eqn{h^{-1}()} such that
#   \deqn{
#     j = h^{-1}(i).
#   }
#   Since the mapping is required to be bijective, it holds that 
#   \eqn{i = h(h^{-1}(i))} and that \eqn{j = h^{-1}(h(j))}. 
#   For example, consider the "reversing" read map function
#   \eqn{h(j)=N*K-j+1}.  The write map function is \eqn{h^{-1}(i)=N*K-i+1}.
#   To verify the bijective property of this map, we see that
#   \eqn{h(h^{-1}(i)) = h(N*K-i+1) = N*K-(N*K-i+1)+1 = i} as well as
#   \eqn{h^{-1}(h(j)) = h^{-1}(N*K-j+1) = N*K-(N*K-j+1)+1 = j}.
# }
#
# \section{Read and write maps in R}{
#   In this package, read and write maps are represented as @integer 
#   @vectors of length \eqn{N*K} with \emph{unique} elements in
#   \eqn{\{1,2,...,N*K\}}.
#   Consider cell and file indices as in previous section.
#
#   For example, the "reversing" read map in previous section can be
#   represented as
#   \preformatted{
#     readMap <- (N*K):1
#   }
#   Given a @vector \code{j} of file indices, the cell indices are
#   the obtained as \code{i = readMap[j]}. 
#   The corresponding write map is
#   \preformatted{
#     writeMap <- (N*K):1
#   }
#   and given a @vector \code{i} of cell indices, the file indices are
#   the obtained as \code{j = writeMap[i]}.
#
#   Note also that the bijective property holds for this mapping, that is
#   \code{i == readMap[writeMap[i]]} and \code{i == writeMap[readMap[i]]} 
#   are both @TRUE.
#
#   Because the mapping is bijective, the write map can be calculated from
#   the read map by:
#   \preformatted{
#     writeMap <- order(readMap)
#   }
#   and vice versa:
#   \preformatted{
#     readMap <- order(writeMap)
#   }
#   Note, the @see "invertMap" method is much faster than \code{order()}.
#
#   Since most algorithms for Affymetrix data are based on probeset (unit)
#   models, it is natural to read data unit by unit.  Thus, to optimize the
#   speed, cells should be stored in contiguous blocks of units.
#   The methods @see "readCdfUnitsWriteMap" can be used to generate a 
#   \emph{write map} from a CDF file such that if the units are read in
#   order, @see "readCelUnits" will read the cells data in order.  
#   Example:
#   \preformatted{
#     Find any CDF file
#     cdfFile <- findCdf()
#
#     # Get the order of cell indices
#     indices <- readCdfCellIndices(cdfFile)
#     indices <- unlist(indices, use.names=FALSE)
#
#     # Get an optimal write map for the CDF file
#     writeMap <- readCdfUnitsWriteMap(cdfFile)
#
#     # Get the read map
#     readMap <- invertMap(writeMap)
#
#     # Validate correctness
#     indices2 <- readMap[indices]    # == 1, 2, 3, ..., N*K
#   }
#
#   \emph{Warning}, do not misunderstand this example.  It can not be used
#   improve the reading speed of default CEL files.  For this, the data in 
#   the CEL files has to be rearranged (by the corresponding write map).
# }
#
# @author
#
# @keyword internal
#*/#########################################################################  

\name{readBpmap}
\alias{readBpmap}
\alias{readBpmapHeader}
\alias{readBpmapSeqinfo}
\title{Parses a Bpmap file}
\description{
  Parses (parts of) a Bpmap (binary probe mapping) file from Affymetrix.
}
\usage{
readBpmap(filename, seqIndices = NULL, readProbeSeq = TRUE, readSeqInfo
= TRUE, readPMXY = TRUE, readMMXY = TRUE, readStartPos = TRUE,
readCenterPos = FALSE, readStrand = TRUE, readMatchScore = FALSE,
readProbeLength = FALSE, verbose = 0)

readBpmapHeader(filename)

readBpmapSeqinfo(filename, seqIndices = NULL, verbose = 0)
}

\arguments{
  \item{filename}{The filename as a character.}
  \item{seqIndices}{A vector of integers, detailing the indices of the
    sequences being read. If \code{NULL}, the entire file is being read.}
  \item{readProbeSeq}{Do we read the probe sequences.}
  \item{readSeqInfo}{Do we read the sequence information (a list
    containing information such as sequence name, number of hits etc.)}
  \item{readPMXY}{Do we read the (x,y) coordinates of the PM-probes.}
  \item{readMMXY}{Do we read the (x,y) coordinates of the MM-probes
    (only relevant if the file has MM information)}
  \item{readStartPos}{Do we read the start position of the probes.}
  \item{readCenterPos}{Do we return the start position of the probes.}
  \item{readStrand}{Do we return the strand of the hits.}
  \item{readMatchScore}{Do we return the matchscore.}
  \item{readProbeLength}{Doe we return the probelength.}
  \item{verbose}{How verbose do we want to be.}
}
\details{
  \code{readBpmap} reads a BPMAP file, which is a binary file containing
  information about a given probe's location in a sequence.
  Here sequence means some kind of reference sequence, typically a
  chromosome or a scaffold. \code{readBpmapHeader} reads the header of
  the BPMAP file, and \code{readBpmapSeqinfo} reads the sequence info of
  the sequences (so this function is merely a convinience function). 
}
\value{
  For \code{readBpmap}: A list of lists, one list for every sequence
  read. The components of 
  the sequence lists, depends on the argument of the function call. For
  \code{readBpmapheader} a list with two components \code{version} and
  \code{numSequences}. For \code{readBpmapSeqinfo} a list of lists
  containing the sequence info.
}

\author{Kasper Daniel Hansen <khansen@stat.berkeley.edu>}

\seealso{\code{\link{tpmap2bpmap}} for information on how to write
  Bpmap files.}

\keyword{file}
\keyword{IO}


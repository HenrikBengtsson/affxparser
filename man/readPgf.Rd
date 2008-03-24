\name{readPgf}
\alias{readPgf}
\title{Parsing a PGF file using Affymetrix Fusion SDK}
\description{
  This function parses a PGF file using the Affymetrix Fusion SDK. PGF
  (probe group) files describe probes present within probe sets, including the
  type (e.g., pm, mm) of the probe and probeset.
}
\usage{
readPgf(file, indices = NULL)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{file}{\code{character(1)} providing a path to the PGF file to be
    input.}
  \item{indices}{\code{integer(n)} a vector of indices of the probesets to
    be read.}
}
\value{
  An list. The \code{header} element is always present; the
  remainder are present when \code{readBody=TRUE}.

  The elements present when \code{readBody=TRUE} describe probe sets,
  atoms, and probes. Elements within probe sets, for instance, are
  coordinated such that the \code{i}th index of one vector (e.g.,
  \code{probesetId}) corresponds to the \code{i}th index of a second
  vector (e.g., \code{probesetType}). The atoms contained within 
  probeset \code{i} are in positions
  \code{probesetStartAtom[i]:(probesetStartAtom[i+1]-1)} of the atom
  vectors. A similar map applies to probes within atoms, using
  \code{atomStartProbe} as the index.

  The PGF file format includes optional elements; these elements are
  always present in the list, but with appropriate default values.

  \item{header}{A list with information about the PGF file. The list
    contains elements described in the PGF file format document
    referenced below.}
  \item{probesetId}{integer vector of probeset identifiers.}
  \item{probesetType}{character vector of probeset types. Types are
    described in the PGF file format document.}
  \item{probesetName}{character vector of probeset names.}
  \item{probesetStartAtom}{integer vector of the start index
    (e.g., in the element \code{atomId} of atoms belonging to this probeset).}

  \item{atomId}{integer vector of atom identifiers.}
  \item{atomExonPosition}{integer vector of probe interrogation position
    relative to the target sequence.}
  \item{atomStartProbe}{integer vector of the start index (e.g., in the
    element \code{probeId} of probes belonging to this atom).}

  \item{probeId}{integer vector of probe identifiers.}
  \item{probeType}{character vector of probe types. Types are described
    in the PGF file format document.}
  \item{probeGcCount}{integer vector of probe GC content.}
  \item{probeLength}{integer vector of probe lengths.}
  \item{probeInterrogationPosition}{integer vector of the position,
    within the probe, at which interrogation occurs.}
  \item{probeSequence}{character vector of the probe sequence.}
}
\author{Martin Morgan \url{mtmorgan@fhcrc.org}}
\seealso{
  \url{https://www.affymetrix.com/support/developer/fusion/File_Format_PGF_aptv161.pdf}
  describes PGF file content.

  The internal function \code{.pgfProbeIndexFromProbesetIndex} provides
  a map between
  the indicies of probe set entries and the indicies of the probes
  contained in the probe set.
}
\keyword{file}
\keyword{IO}

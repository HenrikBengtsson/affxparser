%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  arrangeCelFilesByChipType.R
% 
% by the Rdoc compiler part of the R.oo package.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{arrangeCelFilesByChipType}
\alias{arrangeCelFilesByChipType}


\title{Moves CEL files to subdirectories with names corresponding to the chip types}

\description{
 Moves CEL files to subdirectories with names corresponding to the chip types according to the CEL file headers.
 For instance, a HG_U95Av2 CEL file with pathname "data/foo.CEL"
 will be moved to subdirectory \code{celFiles/HG_U95Av2/}.
}

\usage{
arrangeCelFilesByChipType(pathnames=list.files(pattern = "[.](cel|CEL)$"),
  path="celFiles/", aliases=NULL, ...)
}

\arguments{
 \item{pathnames}{A \code{\link[base]{character}} \code{\link[base]{vector}} of CEL pathnames to be moved.}
 \item{path}{A \code{\link[base]{character}} string specifying the root output directory,
    which in turn will contain chip-type subdirectories.
    All directories will be created, if missing.}
 \item{aliases}{A named \code{\link[base]{character}} string with chip type aliases.
    For instance, \code{aliases=c("Focus"="HG-Focus")} will treat
    a CEL file with chiptype label 'Focus' (early-access name) as
    if it was 'HG-Focus' (offical name).}
 \item{...}{Not used.}
}

\value{
 Returns (invisibly) a named \code{\link[base]{character}} \code{\link[base]{vector}} of the new pathnames
 with the chip types as the names.
 Files that could not be moved or where not valid CEL files
 are set to missing values.
}

\seealso{
 The chip type is inferred from the CEL file header,
 cf. \code{\link{readCelHeader}}().
}

\author{Henrik Bengtsson}



\keyword{programming}
\keyword{internal}

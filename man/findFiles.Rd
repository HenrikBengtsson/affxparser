%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Do not modify this file since it was automatically generated from:
% 
%  
% 
% on Tue Mar 14 18:46:10 2006.
% 
% Generator was the Rdoc class, which is part of the R.oo package written
% by Henrik Bengtsson, 2001-2006.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\name{findFiles}
\alias{findFiles}


\title{Finds one or several files in multiple directories}

\description{
 Finds one or several files in multiple directories.
}

\usage{findFiles(pattern=NULL, paths=NULL, firstOnly=TRUE, ...)}

\arguments{
 \item{pattern}{A regular expression file name pattern to match.}
 \item{paths}{A \code{\link[base]{character}} \code{\link[base]{vector}} of paths to be searched.}
 \item{firstOnly}{If \code{\link[base:logical]{TRUE}}, the method returns as soon as a matching
   file is found, otherwise not.}
 \item{...}{Arguments passed to \code{\link[base]{list.files}}().}
}

\value{
 Returns a \code{\link[base]{vector}} of the full pathnames of the files found.
}

\section{Paths}{
  The \code{paths} argument may also contain paths specified as
  semi-colon (\code{";"}) separated paths, e.g.
  \code{"/usr/;usr/bin/;.;"}.
}

\section{Windows Shortcut links}{
  If package \pkg{R.utils} is available, Windows Shortcut links (*.lnk)
  are recognized and can be used to immitate links to directories
  elsewhere.  For more details, see \code{\link[R.utils]{filePath}}.
}

\author{Henrik Bengtsson \url{http://www.braju.com/R/}}



\keyword{file}
\keyword{IO}
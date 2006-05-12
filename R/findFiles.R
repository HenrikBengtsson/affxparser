########################################################################/**
# @RdocFunction findFiles
#
# @title "Finds one or several files in multiple directories"
#
# \description{
#  @get "title".
# }
#
# @synopsis
#
# \arguments{
#  \item{pattern}{A regular expression file name pattern to match.}
#  \item{paths}{A @character @vector of paths to be searched.}
#  \item{firstOnly}{If @TRUE, the method returns as soon as a matching
#    file is found, otherwise not.}
#  \item{...}{Arguments passed to @see "base::list.files".}
# }
#
# \value{
#  Returns a @vector of the full pathnames of the files found.
# }
#
# \section{Paths}{
#   The \code{paths} argument may also contain paths specified as
#   semi-colon (\code{";"}) separated paths, e.g. 
#   \code{"/usr/;usr/bin/;.;"}.
# }
#
# \section{Windows Shortcut links}{
#   If package \pkg{R.utils} is available, Windows Shortcut links (*.lnk)
#   are recognized and can be used to immitate links to directories
#   elsewhere.  For more details, see @see "R.utils::filePath".
# }
#
# @author
#
# @keyword file
# @keyword IO
# @keyword internal
#**/#######################################################################
findFiles <- function(pattern=NULL, paths=NULL, firstOnly=TRUE, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Local functions
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  splitPaths <- function(paths, ...) {
    if (length(paths) == 0)
      return(NULL);
    # If in format "path1; path2;path3", split it to multiple strings.
    paths <- unlist(strsplit(paths, split=";"));
    paths <- gsub("[ \t]*$", "", gsub("^[ \t]*", "", paths));
    paths <- paths[nchar(paths) > 0];
    if (length(paths) == 0)
      return(NULL);
    paths;
  }


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'paths':
  paths <- splitPaths(paths);
  if (is.null(paths))
    paths <- ".";

  # Argument 'chipType':
  pattern <- as.character(pattern);

  # Argument 'firstOnly':
  firstOnly <- as.logical(firstOnly);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Prepare list of paths to be scanned
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  hasRutils <- suppressWarnings(require(R.utils2, quietly=TRUE));

  # Don't search non-existing paths
  for (kk in seq(along=paths)) {
    path <- paths[kk];
    # Example any '~':s
    path <- file.path(dirname(path), basename(path));
    path <- gsub("^[.][/\\]", "", path);

    # Follow Windows shortcut links?
    if (hasRutils)
      path <- filePath(path, expandLinks="any");

    # Does the path exist and is it a directory
    if (!file.exists(path) || !file.info(path)$isdir)
      path <- NA;

    paths[kk] <- path;
  }
  if (length(paths) > 0)
    paths <- paths[!is.na(paths)];


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Search for files
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  pathnames <- c();
  for (path in paths) {
    files <- list.files(path, pattern=pattern, all.files=TRUE, 
                                                      full.names=TRUE, ...);
    files <- gsub("^[.][/\\]", "", files);
    for (file in files) {
      # Keep only files
      if (!file.info(file)$isdir) {
        if (firstOnly)
          return(file);
        pathnames <- c(pathnames, file);
      }
    }
  }

  pathnames;
} 


############################################################################
# HISTORY:
# 2006-03-14
# o Created from findCdf.R.
############################################################################  

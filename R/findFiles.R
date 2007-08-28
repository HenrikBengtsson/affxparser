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
#  \item{recursive}{If @TRUE, the directory structure is searched 
#    breath-first, in lexicographic order.}
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
findFiles <- function(pattern=NULL, paths=NULL, recursive=FALSE, firstOnly=TRUE, ...) {
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

  # Argument 'pattern':
  if (!is.null(pattern))
    pattern <- as.character(pattern);

  # Argument 'recursive':
  recursive <- as.logical(recursive);

  # Argument 'firstOnly':
  firstOnly <- as.logical(firstOnly);


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Prepare list of paths to be scanned
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  hasRutils <- suppressWarnings(require(R.utils, quietly=TRUE));

  # Don't search the same path twice
  paths <- unique(paths);

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
    files <- list.files(path, all.files=TRUE, full.names=TRUE);

    # Exclude listings that are neither files nor directories
    files <- gsub("^[.][/\\]", "", files);
    files <- files[nchar(files) > 0];
    if (length(files) > 0) {
      excl <- (basename(files) %in% c(".", "..", "/", "\\"));
      files <- files[!excl];
    }

    # Nothing to do?
    if (length(files) == 0)
      next;

    # Expand Windows shortcut links?
    if (hasRutils) {
      files <- sapply(files, FUN=filePath, expandLinks="any", USE.NAMES=FALSE);
    }
 
    # Keep only existing directories
    ok <- sapply(files, FUN=function(file) {
      (file.exists(path) && !is.na(file.info(file)$isdir));
    }, USE.NAMES=FALSE)
    files <- files[ok];

    # Nothing to do?
    if (length(files) == 0)
      next;

    # First search the files, then the directories
    isDir <- sapply(files, FUN=function(file) {
      file.info(file)$isdir;
    }, USE.NAMES=FALSE);

    # In case some files are non-accessible, exclude them
    ok <- (!is.na(isDir));
    files <- files[ok];
    isDir <- isDir[ok];

    # Nothing to do?
    if (length(files) == 0)
      next;

    # Directories and files in lexicographic order
    dirs <- files[isDir];
    files <- files[!isDir];

    # Keep only files that match the filename pattern
    if (!is.null(pattern))
      files <- grep(pattern, files, value=TRUE);

    if (length(files) > 0) {
      files <- sort(files);
      if (firstOnly)
        return(files[1]);

      # Store results
      pathnames <- c(pathnames, files);
    }

    # Search directories recursively?
    if (recursive) {
      if (length(dirs) == 0)
        next;

      for (dir in sort(dirs)) {
        files <- findFiles(pattern=pattern, paths=dir, recursive=recursive,
                                                 firstOnly=firstOnly, ...);
        if (length(files) > 0 && firstOnly)
          return(files[1]);

        pathnames <- c(pathnames, files);
      }
    }
  } # for (path ...)

  pathnames;
} # findFiles()


############################################################################
# HISTORY:
# 2007-08-27
# o Now findFiles(..., recursive=TRUE) does a breath-first search in
#   lexicographic order.
# o Now findFiles() don't search replicated directories.
# 2006-11-01
# o Removed usage of R.utils for now.
# 2006-03-14
# o Created from findCdf.R.
############################################################################  

########################################################################/**
# @RdocFunction arrangeCelFilesByChipType
#
# @title "Moves CEL files to subdirectories with names corresponding to the chip types"
#
# \description{
#  @get "title" according to the CEL file headers.
#  For instance, a HG_U95Av2 CEL file with pathname "data/foo.CEL"
#  will be moved to subdirectory \code{celFiles/HG_U95Av2/}.
# }
#
# @synopsis
#
# \arguments{
#  \item{pathnames}{A @character @vector of CEL pathnames to be moved.}
#  \item{path}{A @character string specifying the root output directory,
#     which in turn will contain chip-type subdirectories.
#     All directories will be created, if missing.}
#  \item{aliases}{A named @character string with chip type aliases.
#     For instance, \code{aliases=c("Focus"="HG-Focus")} will treat
#     a CEL file with chiptype label 'Focus' (early-access name) as
#     if it was 'HG-Focus' (offical name).}
#  \item{...}{Not used.}
# }
#
# \value{
#  Returns (invisibly) a named @character @vector of the new pathnames
#  with the chip types as the names.
#  Files that could not be moved or where not valid CEL files
#  are set to missing values.
# }
#
# \seealso{
#  The chip type is inferred from the CEL file header,
#  cf. @see "readCelHeader".
# }
#
# @author "HB"
#
# @keyword programming
# @keyword internal
#**/#######################################################################
arrangeCelFilesByChipType <- function(pathnames=list.files(pattern="[.](cel|CEL)$"), path="celFiles/", aliases=NULL, ...) {
  requireNamespace("R.utils") || stop("Package not loaded: R.utils");
  Arguments <- R.utils::Arguments
  isFile <- R.utils::isFile
  filePath <- R.utils::filePath


  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  # Argument 'path':
  path <- Arguments$getCharacter(path);

  # Argument 'path':
  if (!is.null(aliases)) {
    aliases <- Arguments$getCharacters(aliases, useNames=TRUE);
    stopifnot(!is.null(names(aliases)));
  }

  pathnamesD <- rep(NA_character_, length=length(pathnames));
  chipTypes <- rep(NA_character_, length=length(pathnames));

  for (ii in seq(along=pathnames)) {
    pathname <- pathnames[ii];

    # Skip non-existing files
    if (!isFile(pathname)) {
      next;
    }

    hdr <- readCelHeader(pathname);
    chipType <- hdr$chiptype;

    # Rename according to alias?
    if (!is.null(aliases)) {
      alias <- aliases[chipType];
      if (!is.na(alias)) {
        chipType <- alias;
      }
    }

    chipTypes[ii] <- chipType;

    filename <- basename(pathname);
    pathD <- filePath(path, chipType);
    pathnameD <- Arguments$getWritablePathname(filename, path=pathD);

    res <- file.rename(from=pathname, to=pathnameD);
    if (res) {
      pathnamesD[ii] <- pathnameD;
    }
  } # for (ii ...)

  names(pathnamesD) <- chipTypes;

  invisible(pathnamesD);
} # arrangeCelFilesByChipType()


############################################################################
# HISTORY:
# 2015-01-06
# o Now using requireNamespace() instead of require().
# 2014-08-25
# o Now using stop() instead of throw().
# 2012-09-01
# o Added argument 'aliases' to arrangeCelFilesByChipType(), e.g.
#   arrangeCelFilesByChipType(..., aliases=c("Focus"="HG-Focus")).
# o BUG FIX: arrangeCelFilesByChipType(pathnames) assumed 'pathnames'
#   were files in the current directory.
# 2012-06-19
# o Created.
############################################################################

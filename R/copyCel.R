########################################################################/**
# @RdocFunction copyCel
#
# @title "Copies a CEL file"
#
# \description{
#  @get "title".
#
#  The file must be a valid CEL file, if not an exception is thrown.
# }
#
# @synopsis
#
# \arguments{
#  \item{from}{The filename of the CEL file to be copied.}
#  \item{to}{The filename of destination file.}
#  \item{overwrite}{If @FALSE and the destination file already exists,
#    an exception is thrown, otherwise not.}
#  \item{...}{Not used.}
# }
#
# \value{
#   Return @TRUE if file was successfully copied, otherwise @FALSE.
# }
#
# \seealso{
#   @see "isCelFile".
# }
#
# @author "HB"
#
# @keyword programming
# @keyword file
# @keyword IO
# @keyword internal
#**/#######################################################################
copyCel <- function(from, to, overwrite=FALSE, ...) {
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Validate arguments
  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  # Argument 'from':
  if (!file.exists(from)) {
    stop("Cannot copy CEL file. File not found: ", from);
  }

  # Argument 'to':
  if (file.exists(to) && !overwrite) {
    stop("Cannot copy CEL file. File already exists: ", to);
  }

  # Check if a CEL file
  if (!isCelFile(from)) {
    stop("Cannot copy CEL file. Source file is not a valid CEL file: ", from);
  }

  file.copy(from, to, overwrite=overwrite);
}


############################################################################
# HISTORY:
# 2006-07-10
# o Added check with isCelFile().
# 2006-06-22
# o Created.
############################################################################  

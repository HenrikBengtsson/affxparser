##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
##############################################################

# Search for some available Calvin CEL files
path <- system.file("rawData", package="AffymetrixDataTestFiles")
files <- findFiles(pattern="[.](cel|CEL)$", path=path, recursive=TRUE, firstOnly=FALSE)
files <- grep("FusionSDK_Test3", files, value=TRUE)
files <- grep("Calvin", files, value=TRUE)
file <- files[1]


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Read a subset of cells
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
indices <- c(1:5, 1250:1500, 450:440)
cel <- readCel(file, indices=indices, readOutliers=TRUE)
str(cel)


# Clean up
rm(files, file, cel)

##############################################################
}                                                     # STOP #
##############################################################

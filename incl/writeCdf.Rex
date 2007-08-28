##############################################################
if (require("AffymetrixDataTestFiles")) {            # START #
############################################################## 

# Find a CDF file
cdfFile <- findCdf("Test3")


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Create an identical CDF structure
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cdfHeader <- readCdfHeader(cdfFile)
pathname <- sprintf("%s-copy.cdf", cdfHeader$chiptype)
if (file.exists(pathname))
  file.remove(pathname)

# Reading all data takes a minute or two...
cdfUnits <- readCdf(cdfFile)
cdfQcUnits <- readCdfQc(cdfFile)

# Writing takes a couple of minutes...
writeCdf(pathname, cdfheader=cdfHeader, cdf=cdfUnits, 
         cdfqc=cdfQcUnits, overwrite=TRUE)


# Validate
value <- cdfUnits
value2 <- readCdf(pathname)
stopifnot(identical(value, value2))

value <- cdfQcUnits
value2 <- readCdfQc(pathname)
stopifnot(identical(value, value2))

value <- cdfHeader
value2 <- readCdfHeader(pathname)
keep <- setdiff(names(value), c("chiptype", "filename"))
value <- value[keep]
value2 <- value2[keep]
stopifnot(identical(value, value2))

# Clean up
rm(cdfFile, pathname, cdfHeader, cdfUnits, cdfQcUnits, value, value2);

##############################################################
}                                                     # STOP #
##############################################################


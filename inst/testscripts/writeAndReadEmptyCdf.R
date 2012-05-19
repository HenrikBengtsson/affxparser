library("affxparser");

# Create a CDF structure for a chip type with 2-by-3 cells
hdr <- list(
  chiptype="Empty2x3",
  nrows=2, ncols=3,
  nunits=0, nqcunits=0, 
  refseq=""
);
units <- qcUnits <- list();

# Write CDF file
pathname <- file.path(tempdir(), "Empty2x3.cdf");
str(pathname);
writeCdf(pathname, cdfheader=hdr, cdf=units, cdfqc=qcUnits, overwrite=TRUE);

# Read CDF header
hdr2 <- readCdfHeader(pathname);
str(hdr2);

# Read CDF units
units2 <- readCdfUnits(pathname);
str(units2);

#############################################################################
# This script will test:
#  findCdf()
#  readCelHeader()
#  readCdfUnitNames()
#  readCelUnits()
#
# and make sure the return the correct values.  A hard-coded data set 
# obtained from Affymetrix gtype_cel_to_pq to two CEPH CEL files is used
# as a reference.
#############################################################################
library(affxparser);
library(R.utils);

# Compare with hard-coded results orginally obtain from Affymetrix'
# gtype_cel_to_pq software
raw <- list(
  "NA06985_Xba_B5_4000090" = list(
    "AFFX-601964"=c(8563, 1647, 621, 640, 9820, 2014, 744, 254, 8747, 1734, 567, 659, 7728, 1235, 886, 656, 9393, 1655, 551, 507, 7005, 387, 680, 479, 7454, 747, 596, 240, 11860, 2021, 2037, 881, 8406, 1046, 941, 303, 10834, 3579, 1202, 456),
    "SNP_A-1650338"=c(6566, 2525, 1549, 1276, 8209, 2373, 883, 1128, 9789, 1006, 949, 932, 2501, 1032, 683, 533, 4716, 2676, 3762, 2911, 3956, 1226, 1404, 1072, 3908, 1419, 1294, 828, 3628, 1213, 783, 654, 4253, 832, 629, 256, 3863, 1581, 719, 593)
  ),
  "NA06991_Xba_B6_3005528" = list(
     "AFFX-601964"=c(10873, 1809, 542, 474, 13135, 2078, 578, 479, 10015, 1740, 698, 286, 10224, 930, 499, 338, 9910, 1263, 741, 304, 7358, 514, 907, 563, 8150, 582, 790, 300, 10368, 2119, 1558, 508, 6528, 1134, 696, 348, 8275, 2936, 884, 569),
     "SNP_A-1650338"=c(9316, 3454, 1093, 1554, 10964, 3288, 1384, 1462, 11233, 1410, 1015, 1129, 4880, 1204, 989, 640, 5136, 1765, 1650, 1703, 3662, 889, 961, 913, 3683, 885, 948, 602, 3511, 1191, 842, 493, 4342, 891, 670, 276, 4400, 1908, 921, 466)
  )
)
raw <- lapply(raw, FUN=function(array) {
  lapply(array, FUN=matrix, nrow=2);
})

arrays <- names(raw);
cel <- lapply(arrays, FUN=function(array) {
  filename <- paste(array, "CEL", sep=".");
  filename <- Arguments$getReadablePathname(filename, path="cel/Xba/", mustExist=TRUE);

  # Find units to be read
  cdfFile <- findCdf(readCelHeader(filename)$chiptype);
  unitNames <- readCdfUnitNames(cdfFile);
  units <- match(names(raw[[1]]), unitNames);

  # Read CEL data
  cel <- readCelUnits(filename, units=units, stratifyBy="pmmm");

  # Convert into gtype_cel_to_pq format
  cel <- lapply(cel, FUN=function(unit) {
    forward <- rbind(unit[[1]]$intensities, unit[[2]]$intensities);
    reverse <- rbind(unit[[3]]$intensities, unit[[4]]$intensities);
    all <- cbind(forward, reverse);
    matrix(all, nrow=2);
  })

  cel;
})
names(cel) <- arrays;

stopifnot(all.equal(cel, raw))

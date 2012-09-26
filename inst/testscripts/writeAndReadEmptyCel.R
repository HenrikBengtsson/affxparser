library("affxparser");

# Create a CEL header for a chip type with 2-by-3 cells
hdr <- list(
  version=4,
  chiptype="Empty2x3",
  rows=2,
  cols=3,
  algorithm="Percentile\nAlgorithm",
  parameters="Percentile:75;CellMargin:2;OutlierHigh:1.500;OutlierLow:1.004;AlgVersion:6.0;FixedCellSize:TRUE;FullFeatureWidth:5;FullFeatureHeight:5;IgnoreOutliersInShiftRows:FALSE;FeatureExtraction:TRUE;PoolWidthExtenstion:2;PoolHeightExtension:2;UseSubgrids:FALSE;RandomizePixels:FALSE;ErrorBasis:StdvMean;StdMult:1.000000",
  cellmargin=2,
  noutliers=0,
  nmasked=0
);
hdr$header <- sprintf("Cols=%d\nRows=%d\nTotalX=%d\nTotalY=%d\nOffsetX=0\nOffsetY=0\nGridCornerUL=223 141\nGridCornerUR=3590 166\nGridCornerLR=3565 3534\nGridCornerLL=198 3508\nAxis-invertX=0\nAxisInvertY=0\nswapXY=0\nDatHeader=[13..25244]  050523MJA_SNP10K-2.0_KJ05:CLS=3666 RWS=3666 XIN=1  YIN=1  VE=30        2.0 09/26/12 10:01:02 50102200  M10   \024  \024 %s.1sq \024  \024  \024  \024  \024  \024  \024  \024  \024 6\nAlgorithm=%sParameters=%s\n", hdr$cols, hdr$rows, hdr$cols, hdr$rows, hdr$chiptype, hdr$algorithm, hdr$parameters);

# Write CDF file
pathname <- file.path(tempdir(), "Empty2x3.cel");
str(pathname);
createCel(pathname, header=hdr, overwrite=TRUE);

# Read CEL header
hdr2 <- readCelHeader(pathname);
str(hdr2);

# Read CEL data
data2 <- readCel(pathname);
str(data2);

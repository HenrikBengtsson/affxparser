cdfHeaderToCelHeader <- function(cdfHeader, sampleName="noname", date=Sys.time(), ..., version="4") {
  # Argument 'version':
  version <- match.arg(version);

  celHeader <- list();

  # File format version
  celHeader$version <- version;

  # Chip dimensions
  celHeader$cols <- cdfHeader$cols;
  celHeader$rows <- cdfHeader$rows;
  celHeader$total <- celHeader$rows * celHeader$cols;

  # Chip type
  celHeader$chiptype <- cdfHeader$chiptype;

  # Miscellaneous
  # Algorithm
  celHeader$algorithm <- "NoAlgorithm";
  pd <- packageDescription("affxparser");
  creator <- sprintf("Creator:%s;Version:%s;", pd$Package, pd$Version);
  celHeader$parameters <- paste(creator, sep="");

  # CEL v3 header
  cols <- celHeader$cols;
  rows <- celHeader$rows;

  datHeader <- list(
    pixelRange="[0..65535]",
    sampleName=sampleName,
    CLS=cols,
    RWS=rows,
    XIN=0,
    YIN=0,
    VE=0,
    scanTemp="",
    laserPower="",
    scanDate=format(date, "%m/%d/%y %H:%M:%S"),
    scanner=list(id="", type=""),
    chipType=celHeader$chiptype
  )
  datHeader <- .wrapDatHeader(datHeader);

  header <- sprintf("Cols=%d\nRows=%d\nTotalX=%d\nTotalY=%d\nOffsetX=0\nOffsetY=0\nGridCornerUL=0 0\nGridCornerUR=%d 0\nGridCornerLR=%d %d\nGridCornerLL=0 %d\nAxis-invertX=0\nAxisInvertY=0\nswapXY=0\nDatHeader=%s\nAlgorithm=%s\nAlgorithmParameters=%s\n", cols, rows, cols, rows, cols, cols, rows, rows, datHeader, celHeader$algorithm, celHeader$parameters);

  celHeader$header <- header;

  # Extras
  celHeader$cellmargin <- 2;
  celHeader$noutliers <- 1;
  celHeader$nmasked <- 1;

  celHeader;
} # cdfHeaderToCelHeader()

.findCcgParent <- function(dataHeader, dataTypeId, ...) { 
  for (parent in dataHeader$parents) {
   if (identical(parent$dataTypeId, dataTypeId)) 
     return(parent);
  }

  NULL;
} # .findCcgParent()


.getCelHeaderVersion <- function(header, ...) {
  version <- header$version;
  if (is.null(version)) {
    version <- header$fileHeader$version;
  }
  if (is.null(version)) {
    throw("Cannot identify header version.  Argument 'header' has an unknown format: ", class(header)[1]);
  }

  version;
} # .getCelHeaderVersion()


# Get the DatHeader from the CCG CEL header
.getCelDatHeader <- function(header, ...) {
  version <- .getCelHeaderVersion(header);
  if (version == 1) {    
    # Command Console Generic (Calvin) format
    dataHeader <- header$dataHeader;
    parent <- .findCcgParent(dataHeader, 
                         dataTypeId="affymetrix-calvin-scan-acquisition");
    datHeader <- parent$parameters[["affymetrix-dat-header"]];
  } else if (version == 3) {
    datHeader <- affxparser:::.unwrapCelHeaderV3String(header)$datHeader;
    datHeader <- affxparser:::.wrapDatHeader(datHeader);
  } else if (version == 4) {    
    datHeader <- affxparser:::.unwrapCelHeaderV4(header)$header$DatHeader;
    datHeader <- affxparser:::.wrapDatHeader(datHeader);
  } else {
    throw("Cannot extract DAT header from CEL header.  Unknown CEL header version: ", version);
  }
 
  datHeader;
} # .getCelDatHeader()



# Extract a CEL header of v3 from the CCG CEL header
.getCelHeaderV3 <- function(header, ...) {
  version <- .getCelHeaderVersion(header);
  if (version == 1) {    
    # Command Console Generic (Calvin) format
    dataHeader <- header$dataHeader;
    params <- dataHeader$parameters;
    # Algorithm parameters
    pattern <- "^affymetrix-algorithm-param-";
    idxs <- grep(pattern, names(params));
    aParams <- params[idxs];
    names(aParams) <- gsub(pattern, "", names(aParams));

    hdr <- NULL;
    rows <- params[["affymetrix-cel-rows"]][1];
    cols <- params[["affymetrix-cel-cols"]][1];
    hdr <- c(hdr, sprintf("Cols=%d\nRows=%d\n", cols, rows));
    hdr <- c(hdr, sprintf("TotalX=%d\nTotalY=%d\n", cols, rows));
    hdr <- c(hdr, sprintf("OffsetX=0\nOffsetY=0\n", 0, 0));

    for (ff in c("UL", "UR", "LR", "LL")) {
      xkey <- sprintf("Grid%sX", ff);
      ykey <- sprintf("Grid%sY", ff);
      hdr <- c(hdr, sprintf("GridCorner%s=%d %d\n", ff, 
                            aParams[[xkey]][1], aParams[[ykey]][1]));
    }
    hdr <- c(hdr, sprintf("Axis-invertX=%d\nAxisInvertY=%d\n", 0, 0));
    hdr <- c(hdr, sprintf("swapXY=%d\n", 0));
    parent <- .findCcgParent(dataHeader, 
                         dataTypeId="affymetrix-calvin-scan-acquisition");
    datHeader <- parent$parameters[["affymetrix-dat-header"]];
    hdr <- c(hdr, sprintf("DatHeader=%s\n", datHeader));

    hdr <- c(hdr, sprintf("Algorithm=%s\n", params[["affymetrix-algorithm-name"]]));

    excl <- grep("^Grid", names(aParams));
    aParams <- aParams[-excl];
    aParams <- sapply(aParams, FUN=function(x) x[1]);
    aParams <- paste(names(aParams), aParams, sep=":");
    aParams <- paste(aParams, collapse=";");
    hdr <- c(hdr, sprintf("AlgorithmParameters=%s\n", aParams));

    hdr <- paste(hdr, collapse="");
    
    headerV3 <- hdr;
  } else if (version == 3) {
    # Nothing to do.
    headerV3 <- header;
    datHeader <- affxparser:::.wrapDatHeader(datHeader);
  } else if (version == 4) {
    # To do: Create a v3 header from scratch (for consistency).
    headerV3 <- header$header;
  } else {
    throw("Cannot extract CEL header of v3 from CEL header.  Unknown CEL header version: ", version);
  }
 
  headerV3;
} # .getCelHeaderV3()


.getCelHeaderV4 <- function(header, ...) {
  version <- .getCelHeaderVersion(header);
  if (version == 1) {
    # Calvin CEL header?
    if (is.null(header$fileHeader)) {
      # Re-read the CEL CCG v1 header 
      headerV4 <- header;
      header <- readCcgHeader(headerV4$filename);
    } else {
      # Re-read the CEL v4 header
      headerV4 <- readCelHeader(header$filename);
    }
    # Append CEL v3 header
    headerV4$header <- .getCelHeaderV3(header);
    headerV4 <- .unwrapCelHeaderV4(headerV4);
    headerV4 <- .wrapCelHeaderV4(headerV4);
  } else if (version == 3) {
    throw("Cannot get CEL header of v4 from CEL header of v3.  Non-implemented feature.");
  } else if (version == 4) {
    headerV4 <- .wrapCelHeaderV4(.unwrapCelHeaderV4(header));
  } else {
    throw("Cannot extract CEL header of v3 from CEL header.  Unknown CEL header version: ", version);
  }

  headerV4;    
} # .getCelHeaderV4()


############################################################################
# HISTORY:
# 2007-08-16
# o Added .getCelHeaderV4(). Verified to work with CEL v1 & v4 headers.
# o Added .getCelHeaderV3(). Verified to work with CEL v1, v3 & v4 headers.
# o Added .getCelDatHeader(). Verified to work with CEL v1 & v4 headers.
# o Created.
############################################################################  

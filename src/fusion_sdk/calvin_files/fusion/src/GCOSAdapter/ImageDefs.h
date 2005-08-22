/**                           IMAGEDEFS.H
//-------------------------------------------------------------------
//
//  LANGUAGE: Visual C++ (with Microsoft Foundation Classes)
//
//  DESCRIPTION:  Image Data Class for DAT File I/O
//
//  CONTENTS:  definition of constants and structures
//
//-------------------------------------------------------------------
// */
////////////////////////////////////////////////////////////////////////////////
//	© Copyright 1997-2004, Affymetrix, Inc. 
//	All rights reserved. Confidential. Except as pursuant to a written agreement
//	with Affymetrix, this software may not be used or distributed. This software
//	may be covered by one or more patents.
//	Author: John Stephens
//	Version History: Version 7.0
//	"GeneChip", "Affymetrix", and the Affymetrix logos, and Affymetrix user
//	interfaces are trademarks used by Affymetrix.
//
//	01/29/05 Defect 10490 DEFECT_ADDRESSED  Instrument control does not record wavelength in DAT header Author: jsteph
//
/////////////////////////////////////////////////////////////////////////////////

// ImageDefs.h

#define MAXMEMSIZE  1024*1024L
#define MAXRWSIZE   64*1024L
#define DATASIZE sizeof(WORD)

// chipinfo defines
const int N_INFOSTR  = 10;
const int INFOSTRLEN = 20;
const char DELIMCHAR = '\x14';
// indices of chipinfo items
const int iChipType = 1;
const int iPixelSize = N_INFOSTR-1;
const int iLaserSpotSize = N_INFOSTR-2;
const int iArcRadius = N_INFOSTR-3;
const int iFilter = N_INFOSTR-4;

#define AMXFILETYPE 252

#define ORIGINAL_FILE_VERSION		0 // File version for files created by DatIO
#define FILE_VERSION_WITH_FOOTER	5 // The starting file version with footer info
#define CURRENT_FILE_VERSION		7 // File version for files created by DatIO7

// Image Data File Header

// set byte packing to 1 for this structure                                                    
#pragma pack(push, 1)

struct AMXHEADER  {
    BYTE nType;
    WORD nPixels;
    WORD nLines;
    DWORD dwTotalPoints;
    DWORD minData, maxData;
    double meanData, stdDev;
    char szPixels [9];
    char szLines [9];
    char szPixelSize [7];
    char szLineSpace [7];
    char szScanSpeed [6];
    char szTemperature [7];
    char szLaserPower [4];
    char szDateTime [18];
    char szChipInfo [220];
    double AveDCOffset;
    double OffsetStdDev;
    unsigned int nDCSamples;
    struct {
      POINTS upperleft;
      POINTS upperright;
      POINTS lowerright;
      POINTS lowerleft;
      } GridCoords;
    WORD Margin;
    char szTestName [154];
};

#define HEADERSIZE sizeof(AMXHEADER)

#define MAXBARCODELEN 64		// From GeneChip

struct AMXFOOTER  {
    WORD            nVersion;
    BOOL            bSubPixelsGrid;
    SUBPIXELS       SubPixels;
    char            szBarcode[MAXBARCODELEN];
    WORD            nNumSubGrids;
};

#define FOOTERSIZE sizeof(AMXFOOTER)

#pragma pack(pop)

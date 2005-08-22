/**                           IMAGEDATA.H
//-------------------------------------------------------------------
//
//  LANGUAGE: Visual C++ (with Microsoft Foundation Classes)
//
//  DESCRIPTION:  Image Data Class for DAT File I/O
//
//  CONTENTS:  definition of the image data class ImageData
//
//-------------------------------------------------------------------
// */
////////////////////////////////////////////////////////////////////////////////
//	© Copyright 1997-2004, Affymetrix, Inc. 
//	All rights reserved. Confidential. Except as pursuant to a written agreement
//	with Affymetrix, this software may not be used or distributed. This software
//	may be covered by one or more patents.
//	Author: John Stephens
//	Version History: Version 7
//	"GeneChip", "Affymetrix", and the Affymetrix logos, and Affymetrix user
//	interfaces are trademarks used by Affymetrix.
//
//	01/29/05 Defect 10490 DEFECT_ADDRESSED  Instrument control does not record wavelength in DAT header Author: jsteph
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGEDATA_H
#define __IMAGEDATA_H

#define DLLEXPORT __declspec(dllexport)

// maximum file mapping size = 200 MB
#define MAXFILEMAPSIZE 1024*1024*200L


typedef struct {
    POINT upperleft;
    POINT upperright;
    POINT lowerright;
    POINT lowerleft;
} GRIDCOORDS;

struct PIXEL_COORD
{
	float fx;
	float fy;
};

typedef struct 
{
	PIXEL_COORD	upperleft;
	PIXEL_COORD	upperright;
	PIXEL_COORD	lowerleft;
	PIXEL_COORD	lowerright;
} SUBPIXELS;

#ifndef SUBGRID_DEFINE
#define SUBGRID_DEFINE
struct FLOAT_COORD
{
	float fx;
	float fy;
	bool operator==(FLOAT_COORD& rhs);
};

inline bool FLOAT_COORD::operator==(FLOAT_COORD& rhs) {return( rhs.fx == fx && rhs.fy == fy );}

struct FLOAT_RECT 
{
	FLOAT_COORD upperleft;
	FLOAT_COORD upperright;
	FLOAT_COORD lowerleft;
    FLOAT_COORD lowerright;
    bool operator==(FLOAT_RECT& rhs);
};

inline bool FLOAT_RECT::operator==(FLOAT_RECT& rhs)
{
	return (rhs.lowerleft == lowerleft && rhs.lowerright == lowerright &&
			rhs.upperleft == upperleft && rhs.upperright == upperright);
}

struct SUBGRID {
	int row;
	int col;
	FLOAT_RECT pixelgrid;
	CRect cellgrid;
	bool operator==( SUBGRID& rhs );
};

inline bool SUBGRID::operator==( SUBGRID& rhs )
{
	return( rhs.row == row && rhs.col == col &&
			rhs.pixelgrid == pixelgrid && rhs.cellgrid == cellgrid );
}

#endif

#define SUBGRIDSIZE sizeof(SUBGRID)
typedef CArray<SUBGRID, SUBGRID> SUBGRID_ARRAY;

typedef enum RWAccess { READ = 1, WRITE, READ_AND_WRITE };
typedef enum ShareMode { EXCLUSIVE, ALLOWREAD, ALLOWWRITE, ALLOWREAD_AND_WRITE };

class ImageData
{
private:
    // copy constructor and assignment operator are declared
    // but not implemented to prevent copying this class
	ImageData(const ImageData& imageSrc);          // no implementation
	void operator=(const ImageData& imageSrc);     // no implementation
protected:
    int nPixels;                // number of pixels per line
    int nLines;                 // number of lines in image
    WORD * lpData;
    void * lpFileMap;
    DWORD dwMapMode;
    HANDLE hFile, hFileMap;
    int nFirstLineMapped;
    int nLinesMapped;
    CString strPathName;
    CStringArray ChipInfoStr;
    CString strChipInfo;
    CString strScanDate;
    CString strScanInfo;
    CString strScannerID;
	CString strScannerType;
    CString strExptName;
    CString strImageName;
	WORD nNumSubGrids;
	WORD nOldNumSubGrids;
	SUBGRID_ARRAY SubGrids;

public:
    WORD minData, maxData;
    double meanData, stddev;
    int PixelSize;
    int Orientation;
    int ScanSpeed;
    int Power, Temp;
    double AvgOffset, OffsetStddev;
    UINT nOffsetSamples;
    GRIDCOORDS CellGrid;
    WORD CellMargin;
    double fPixelSize;
    double fArcRadius;
    double fLaserSpotSize;
    int Filter;
	WORD nVersion;
	BOOL bSubPixelsGrid;
	SUBPIXELS SubPixels;

protected:
    BOOL AllocDataMem ();
    void FreeDataMem ();

    BOOL MapFile (DWORD dwSize, UINT mapmode);
    void RemapFile (UINT mapmode);
    void UnmapFile ();

    void GetHeaderInfo (struct AMXHEADER * pHeader);
    void SetHeaderInfo (struct AMXHEADER * pHeader);

    void ReadHeader (CFile& file);
    void ReadData (CFile& file);
    void WriteHeader (CFile& file);
    void WriteData (CFile& file);

    void GetFooterInfo (struct AMXFOOTER * pFooter);
    void SetFooterInfo (struct AMXFOOTER * pFooter);
    void ReadFooter (CFile& file);
    void WriteFooter (CFile& file);
    void ReadSubGrids (CFile& file);
    void WriteSubGrids (CFile& file);

    BOOL ReadFile ();
    BOOL WriteFile ();

public:
    DLLEXPORT ImageData ();
    DLLEXPORT virtual ~ImageData ();

    WORD * GetDataPtr () { return lpData; }
    BOOL IsMappedFile () const { return (lpFileMap != NULL); }
    int FirstLineMapped () const { return nFirstLineMapped; }
    int LinesMapped () const { return nLinesMapped; }
    int xSize () const { return nPixels; }
    int ySize () const { return nLines; }
    const char * GetChipInfo (int index) { return ChipInfoStr[index]; }
    const char * GetChipType () const { return ChipInfoStr[1]; }
    const char * GetExperimentName () const { return strExptName; }
    const char * GetName () const { return strImageName; }
    const char * GetScanDate () const { return strScanDate; }
    const char * GetScannerID () const { return strScannerID; }
	const char * GetScannerType () const { return strScannerType; }
    DLLEXPORT const char * GetChipInfo ();
    DLLEXPORT const char * GetScanInfo ();
    DLLEXPORT void SetChipInfo (int nField, const char * pszChipInfo);
    DLLEXPORT void SetChipType (const char * pszChipType);
    DLLEXPORT void SetExperimentName (const char * pszExptName);
    DLLEXPORT void SetName (const char * pszImageName);
    DLLEXPORT void SetScanDate (const char * pszDate, const char * pszTime = NULL);
    DLLEXPORT void SetScannerID (const char * pszScannerID);
    DLLEXPORT void SetScannerType (const char * pszScannerType);
    DLLEXPORT void SetSize (int xSize, int ySize);

    DLLEXPORT void Close ();
    DLLEXPORT BOOL Create (int xSize, int ySize);
    DLLEXPORT BOOL Delete ();
    DLLEXPORT BOOL Exists ();
    DLLEXPORT BOOL MapData (int nFirstLine, int nLinesToMap);
    DLLEXPORT BOOL Open (RWAccess rwAccess, ShareMode shareMode);
    DLLEXPORT BOOL Read ();
    DLLEXPORT BOOL ReadHeader ();
    DLLEXPORT BOOL Write ();
    DLLEXPORT BOOL WriteHeader ();
    DLLEXPORT BOOL Rename (const char * pszNewName);

	// New functions for subgrids
	int GetNumSubGrids () const { return SubGrids.GetSize(); }
	DLLEXPORT BOOL GetSubGrid(/*[in]*/ int nIndex, /*[ref]*/ SUBGRID *pSubGrid);
	DLLEXPORT BOOL GetSubGrid(/*[in]*/ int nRow, /*[in]*/ int nCol, /*[ref]*/ SUBGRID *pSubGrid);
	DLLEXPORT BOOL AddSubGrid(/*[ref]*/ SUBGRID *pSubGrid);
	DLLEXPORT BOOL UpdateSubGrid(/*[ref]*/ SUBGRID *pSubGrid);
	DLLEXPORT void RemoveSubGrids();
};


DLLEXPORT __int64 GetImageFileSize (int nPixels, int nLines);

#endif // __IMAGEDATA_H

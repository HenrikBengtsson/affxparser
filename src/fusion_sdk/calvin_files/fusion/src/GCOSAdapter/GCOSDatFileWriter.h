/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
/////////////////////////////////////////////////////////////////

#ifndef _GCOSDatFileWriter_HEADER_
#define _GCOSDatFileWriter_HEADER_

#include "FusionDatFileWriter.h"
#include "ImageData.h"
#include <vector>

/*! \file GCOSDatFileWriter.h This file defines GCOS DAT file writer that implements the Fusion DAT file writer interface.
 *  This is Windows only.
 */

namespace affymetrix_fusion_io
{

/*! Writes the GCOSDatData object to the file */
class GCOSDatFileWriter : public FusionDatFileWriter
{
	/*! Constructor*/
  GCOSDatFileWriter();
	/*! Destructor */
  virtual ~GCOSDatFileWriter();

public:
	/*! Create a GCOSDatFileWriter object
	 *	@return A GCOSDatFileWriter pointer
	 */
	static GCOSDatFileWriter* Create() { return new GCOSDatFileWriter; }

	/*! Deletes the FusionDatWriter object.  Call this when done with the object.
	 */
	virtual void Delete() { delete this; }

public:
	/*! Opens the writer.
	 *	This creates and opens the file and writes the header.  It closes a previously opened file.
	 *	@param data. Reference to the data objec to write
	 *	@return true if it succeeded.  Method may also throw exceptions.
	 *	@exception affymetrix_calvin_exceptions::FileCreateException
	 */
	virtual bool Open(FusionDatData& data);

	/*! Closes the writer.
	 *	This updates and closes the file.
	 *	@return true if it succeeded.  Method may also throw exceptions.
	 */
	virtual bool Close();

	/*! Maps the pixel data for access starting at firstRow and continuing for rowsToMap
	 *	@param firstRow First row to map for access
	 *	@param rowsToMap Requested number of rows to map.  Use GetRowsMapped for number of rows actually mapped.
	 */
	virtual void MapData(int32_t firstRow, int32_t rowsToMap);
	/*! Get a pointer to the pixel data that has been mapped for access
	 *	@return Pointer to the mapped data
	 */
	virtual u_int16_t* GetMappedPixelDataPtr();
	/*! Get the first row that has been mapped
	 *	@return Index of the first row mapped for access
	 */
	virtual int32_t GetFirstRowMapped();
	/*! Get the number of rows that have been mapped.
	 *	@return The number of rows mapped for access
	 */
	virtual int32_t GetRowsMapped();
	/*! Get the suggested maximum number of rows to map.
	 *	@return The suggested maximum number of rows to map
	 */
	virtual int32_t GetMaxRowsToMap();

	/*! Writes the min and max pixel intensities to the file
	 *	@param min The minimum pixel intensity
	 *	@param max The maximum pixel intensity
	 *	@exception affymetrix_calvin_exceptions::OutOfRangeParameterException
	 */
	virtual void SetMinMaxData(u_int16_t min, u_int16_t max);
	/*! Writes the average pixel intensity to the file
	 *	@param value The average pixel intensities
	 */
	virtual void SetAvgData(float value) { avgData = value; }
	/*! Writes the standard deviation to the file
	 *	@param value The standard deviation of the pixel intensities
	 */
	virtual void SetStdev(float value) { stdev = value; }

protected:
	bool OpenFile();
	bool WriteCompleteHeader();

	int GetOrientation();
	std::string GetDate();
	std::string GetTime();
	double GetArcRadius();
	double GetLaserSpotSize();
	std::string GetExperimentName();

private:
	/*! DATIO7 object */
	ImageData datio;
	/*! Data object to write */
	FusionDatData* datData;
	/*! Indicates that the writer has been opened */
	bool isOpen;
	/*! Line number of the start of the mapped view */
	u_int32_t startLine;	// read this from datio
	/*! Offset into the current map */
	u_int32_t mapOffset;
	/*! Min pixel intensity data */
	u_int16_t minData;
	/*! Max pixel intensity data */
	u_int16_t maxData;
	/*! Average pixel intensity data */
	float avgData;
	/*! Standard deviation of the pixel intensity data */
	float stdev;
};

}
#endif // _AffymetrixFusionCelFile_HEADER_
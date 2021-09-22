////////////////////////////////////////////////////////////////
//
// Copyright (C) 1989, 1991 Free Software Foundation, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License 
// (version 2.1) as published by the Free Software Foundation.
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
////////////////////////////////////////////////////////////////

#ifndef _UTIL_CALVINTOTEXT_H_
#define _UTIL_CALVINTOTEXT_H_

//
#include "util/AffxBinaryFile.h"
#include "util/AffxByteArray.h"
#include "util/AffxString.h"
#include "util/Err.h"
#include "util/Guid.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include "calvin_files/parameter/src/Parameter.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/utils/src/AffyStlCollectionTypes.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include <fstream>
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;



class CalvinToTextFile
{
public:

    CalvinToTextFile();
    virtual ~CalvinToTextFile();

    bool open(const AffxString& strFileName);

    bool isOpen();
    void write(const char *psz);
    void writeLine(const AffxString& str);
    void close(void);

protected:
    
    std::fstream* m_pfileStream;
    char* m_cb;

};

class CalvinToText
{
protected:
	AffxString m_strFileNameIn;
	AffxString m_strFileNameOut;
	CalvinToTextFile m_file;
	bool m_bHeader;
	int m_iXChromosome;
	int m_iYChromosome;
public:
	CalvinToText() 
	{
		m_bHeader = false;
		m_iXChromosome = -1;
		m_iYChromosome = -1;
	}

	void run(const std::string& strFileNameIn, 
           const std::string& strFileNameOut,
           bool bHeader,
           bool bBody, 
           bool bNewGuid);

protected:
	void error(const AffxString& strMessage);
	/*
	 * Read the file into a data object.
	 */
	bool ReadFile(const std::string &fileName, GenericData &gdata);

	/*
	 * Output the data to the command line.
	 */
	void OutputData(GenericData& gdata, int groupIndex, int setIndex, int startRow, int endRow);

	/*
	 * Output the file header parameters.
	 */
	void OutputFileHeaderParameters(GenericData &gdata, bool bNewGuid);
	/*
	 * Output the header parameters.
	 */
	void OutputParameters(ParameterNameValueTypeIt &begin, ParameterNameValueTypeIt &end);
	/*
	 * Output the header parameters.
	 */
	void OutputParameters(ParameterNameValueTypeConstIt &begin, ParameterNameValueTypeConstIt &end);

	/*
	 * Output the data header parameters.
	 */
	void OutputDataHeaderParameters(GenericDataHeader *hdr, bool bNewGuid);

	/*
	 * Output the set information, only the header.
	 */
	void OutputSetInfo(GenericData &gdata, int groupIndex, int setIndex);

	/*
	 * Output the group information.
	 */
	void OutputGroupInfo(GenericData &gdata, const std::wstring &groupName, int groupIndex);

	/*
	 * Output the groups and sets.
	 */
	void OutputGroupsAndSets(GenericData &gdata);

	/*
	 * Output the header information.
	 */
	void OutputHeader(const std::string &strFileName, affymetrix_calvin_io::GenericData& genericData, bool bNewGuid);

	bool isCalvinFile(const AffxString& strFileName);

};

#endif // _UTIL_CALVINTOTEXT_H_

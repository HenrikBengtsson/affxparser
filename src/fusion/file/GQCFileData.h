////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 Affymetrix, Inc.
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

#ifndef _GQCFileData_HEADER_
#define _GQCFileData_HEADER_

#include "calvin_files/parameter/src/Parameter.h"
//
#include <cstring>
#include <string>
//

namespace affxgqc
{

/*! Used to store information in a GQC file. */
class GQCFileData
{
public:
    /*! Constructor */
    GQCFileData();
    
    /*! Destructor */
    ~GQCFileData();

    /*! Clear the data. */
    void Clear();

    /*! The QC analysis results. */
    affymetrix_calvin_parameter::ParameterNameValuePairList &QCResults() { return qcResults; }

    /*! The sample signature */
    affymetrix_calvin_parameter::ParameterNameValuePairList &SampleSignature() { return sampleSignature; }

    /*! The analysis parameters */
    affymetrix_calvin_parameter::ParameterNameValuePairList &AnalysisParameters() { return analysisParameters; }

	/*! Reads the entire contents of the file.
	 * @param fileName The name of the file to read.
     * @return true if successful
	 */
	bool Read(const std::string &fileName);

	/*! Write the data to the file.
	 * @param fileName The name of the file to read.
     * @return true if successful
	 */
	bool Write(const std::string &fileName);

private:
    /*! The QC analysis results. */
    affymetrix_calvin_parameter::ParameterNameValuePairList qcResults;

    /*! The sample signature */
    affymetrix_calvin_parameter::ParameterNameValuePairList sampleSignature;

    /*! The analysis parameters */
    affymetrix_calvin_parameter::ParameterNameValuePairList analysisParameters;

};

};


#endif

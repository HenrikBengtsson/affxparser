////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
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

#ifndef _FusionCHPMultiDataAccessor_HEADER_
#define _FusionCHPMultiDataAccessor_HEADER_

#include <calvin_files/portability/src/AffymetrixBaseTypes.h>
//
#include <cstring>
#include <map>
#include <string>
#include <vector>
//

namespace affymetrix_fusion_io
{

/*! This class provides a simple interface to extract SNP calls and confidence values from a multi data CHP file. */
class FusionCHPMultiDataAccessor
{
private:
    /*! The list of CHP file names to extract the data from. */
    std::vector<std::string> chpFileNames;

    /*! A map of SNP name to CHP file index. */
    std::map<std::string, int> snpNameIndexMap;

public:
    /*! Constructor
     */
    FusionCHPMultiDataAccessor();

    /*! Destructor */
    ~FusionCHPMultiDataAccessor();

    /*! Initialize the map of SNP names to indicies.
     * @param chps The list of CHP files to extract data from.
     */
    bool Initialize(const std::vector<std::string> &chps);

    /*! Extract the calls and confidences for each input SNP.
     * @param snps The list of snps to extract data for.
     * @param calls A matrix to hold the calls.
     * @param confidences A matrix to hold the confidence values.
     */
    void ExtractData(const std::vector<std::string> &snps, std::vector<std::vector<u_int8_t> > &calls, std::vector<std::vector<float> > &confidences);
};

}

#endif

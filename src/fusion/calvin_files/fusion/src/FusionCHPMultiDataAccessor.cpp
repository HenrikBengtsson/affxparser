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

#include "calvin_files/fusion/src/FusionCHPMultiDataAccessor.h"
//
#include "calvin_files/fusion/src/FusionCHPData.h"
#include "calvin_files/fusion/src/FusionCHPMultiDataData.h"
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace std;

FusionCHPMultiDataAccessor::FusionCHPMultiDataAccessor()
{
}

FusionCHPMultiDataAccessor::~FusionCHPMultiDataAccessor()
{
}

bool FusionCHPMultiDataAccessor::Initialize(const vector<string> &chps)
{
    // Clear the map
    snpNameIndexMap.clear();

    // Store the chp file names.
    chpFileNames = chps;
    if (chps.size() == 0)
        return false;

    // Read the first chp file.
    FusionCHPData *chp = FusionCHPDataReg::Read(chpFileNames[0]);
    if (chp == NULL)
        return false;
    FusionCHPMultiDataData *mchp = FusionCHPMultiDataData::FromBase(chp);
    if (mchp == NULL)
    {
        delete chp;
        return false;
    }

    // Extract the probe set names
    int n = mchp->GetEntryCount(GenotypeMultiDataType);
    for (int i=0; i<n; i++)
        snpNameIndexMap[mchp->GetProbeSetName(GenotypeMultiDataType, i)] = i;

    // Close the file and return
    delete mchp;
    return true;
}

void FusionCHPMultiDataAccessor::ExtractData(const vector<string> &snps, vector<vector<u_int8_t> > &calls, vector<vector<float> > &confidences)
{
    int nchps = (int)chpFileNames.size();
    calls.resize(nchps);
    confidences.resize(nchps);

    // Create a vector of CHP file indicies given the SNP names. This is
    // done to reduce the number of lookups in the map object.
    int nsnps = (int)snps.size();
    vector<int> snpIndicies(nsnps);
    for (int isnp=0; isnp<nsnps; isnp++)
    {
        snpIndicies[isnp] = snpNameIndexMap[snps[isnp]];
    }

    // Loop over the chp files and extract the snp data
    FusionCHPData *chp = NULL;
    FusionCHPMultiDataData *mchp = NULL;
    for (int ichp=0; ichp<nchps; ichp++)
    {
        // Open the CHP file
        chp = FusionCHPDataReg::Read(chpFileNames[ichp]);
        mchp = FusionCHPMultiDataData::FromBase(chp);
        if (mchp == NULL)
        {
            calls[ichp].clear();
            confidences[ichp].clear();
            delete chp;
            chp = NULL;
            mchp = NULL;
            continue;
        }

        // Extract the data
        calls[ichp].resize(nsnps);
        confidences[ichp].resize(nsnps);
        for (int isnp=0; isnp<nsnps; isnp++)
        {
            calls[ichp][isnp] = mchp->GetGenoCall(GenotypeMultiDataType, snpIndicies[isnp]);
            confidences[ichp][isnp] = mchp->GetGenoConfidence(GenotypeMultiDataType, snpIndicies[isnp]);
        }

        // Close the file
        delete mchp;
   }
}


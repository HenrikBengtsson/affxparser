////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
////////////////////////////////////////////////////////////////

/**
 * @file   CHPInfo.h
 * @author David Le
 * @date   Wed Mar 15 12:46:53 2006
 * 
 * @brief Stores CHP information for reporter analyzing genotyping probe sets that outputs chp files.
 */

#ifndef CHIPINFO_H
#define CHIPINFO_H

namespace affxchp
{
class CHPInfo {
  public:
    /** Constructor. */
    CHPInfo() {
      m_NumRows = 0;
      m_NumCols = 0;
      m_NumProbeSets = 0;
      m_ProbeSetType = affxcdf::UnknownProbeSetType;
      m_ProbesetNames = NULL;
	  m_ProbesetGroups = NULL;
    }

    void addParam(const std::string &key, const std::string &value) {
      m_ParamNames.push_back(key);
      m_ParamValues.push_back(value);
    }

    /// How many rows are on the grid of the chip.
    int m_NumRows;
    /// How many columns are the grid of the chip.
    int m_NumCols;
    /// How many probesets are on the chip and in cdf file. This is
    /// critical as probesets must be the exact same in number and order
    /// as in the cdf file.
    int m_NumProbeSets;
    /// What type of chp file is this? For our purposes here it is genotyping
    affxcdf::GeneChipProbeSetType m_ProbeSetType;
    /// What chip is this?
    std::string m_ChipType;
    /// What is our id for GCOS?
    std::string m_ProgID;
    /// What is the name of the program
    std::string m_ProgramName;
    /// What program created this chp file.
    std::string m_ProgramVersion;
    /// What company created this chp file
    std::string m_ProgramCompany;
    /// What algorithm are we reporting and what version.
    std::string m_AlgName;
    std::string m_AlgVersion;
    std::string m_AnalysisGuid;
    std::string m_ExecGuid;
    /// These next two vectors are matched pairs, one contains the
    /// keys and the other the values. Would use a map, but want to be
    /// guaranteed of the order.
    std::vector<std::string> m_ParamNames;
    std::vector<std::string> m_ParamValues;
    /// Vector of probeset names and groups as they should appear in the chp file.
    /// this memory is owned elsewhere as it is relatively large in size.
    std::vector<std::string> *m_ProbesetNames;
	std::vector<ProbeSetGroup *> *m_ProbesetGroups;
};

}

#endif /* CHIPINFO_H */

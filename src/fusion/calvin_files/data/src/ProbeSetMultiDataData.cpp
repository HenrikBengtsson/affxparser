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

//
#include "calvin_files/data/src/ProbeSetMultiDataData.h"
//
#include <cstdlib>
#include <sstream>
//

u_int8_t affymetrix_calvin_data::ChromosomeFromString(const std::string& chr)
{
  int chrValue = atoi(chr.c_str());
  if (chrValue == 0)
    {
      if (chr == "X" || chr == "x")
        chrValue = X_CHR;
      else if (chr == "Y" || chr == "y")
        chrValue = Y_CHR;
      else if (chr == "MT" || chr == "mt")
        chrValue = MT_CHR;
      else
        chrValue = NO_CHR;
    }
  return chrValue;
}

std::string affymetrix_calvin_data::ChromosomeToString(u_int8_t chr)
{
  if (chr == X_CHR)
    return "X";
  else if (chr == Y_CHR)
    return "Y";
  else if (chr == MT_CHR)
    return "MT";
  else if (chr == NO_CHR)
    return "-";
  else
    {
      std::ostringstream str;
      str << (int)chr;
      return str.str();
    }
}

u_int8_t affymetrix_calvin_data::CytoCallFromString(const std::string &call)
{
    if (call == "A")
        return CYTO_ABSENT_CALL;

    else if (call == "P")
        return CYTO_PRESENT_CALL;

    else
        return CYTO_NO_CALL;
}

std::string affymetrix_calvin_data::CytoCallToString(u_int8_t call)
{
    switch (call)
    {
    case CYTO_ABSENT_CALL:
        return "A";
        break;

    case CYTO_PRESENT_CALL:
        return "P";
        break;

    default:
        return "NC";
        break;
    }
}

u_int8_t affymetrix_calvin_data::GenotypeCallFromString(const std::string &call)
{
    if (call == "A" || call == "AA")
        return SNP_AA_CALL;

    else if (call == "B" || call == "BB")
        return SNP_BB_CALL;

	else if (call == "AB")
        return SNP_AB_CALL;

    else
        return SNP_NO_CALL;
}

std::string affymetrix_calvin_data::GenotypeCallToString(u_int8_t call)
{
    switch (call)
    {
    case SNP_AA_CALL:
        return "A";
        break;

    case SNP_AB_CALL:
        return "AB";
        break;

    case SNP_BB_CALL:
        return "BB";
        break;

    default:
        return "No Call";
        break;
    }
}


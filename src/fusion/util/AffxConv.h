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

/**
 * @file AffxConv.h
 *
 * @brief This header contains some data conversion routines. 
 */
/**
 * @brief  Some data conversion routines. 
 * 
 */
      
#ifndef __affxconv_h__
#define __affxconv_h__ 

#include <cstring>
#include <string>
//
  
// NOTE: for visual access purposes only
class AffxConv
{
public:
    AffxConv();
};

	std::string getUnsignedInt(unsigned int i, bool bCommas = false);
	std::string getInt(int i, bool bCommas = false); 
	int getInt(const std::string& str);
	unsigned int getUnsignedInt(const std::string& str);
	std::string getInt(int i, unsigned int iLength); 
	              
	std::string getDouble(double d);
	std::string getDouble(double d, int iDecimalPlaces, bool bCommas = false);
	double getDouble(const std::string& strDouble);
	
	std::string formatString(const char* szValue, int iDecimalPlaces, bool bCommas = false);
	
	std::string getMonth(int iMonth); 
	int getMonth(const std::string& strMonth); 
	
	double roundDouble(double d, int iDecimalPlaces = 0);
	
	std::string likeString(const std::string& str);

         
#endif // __affxconv_h__ 

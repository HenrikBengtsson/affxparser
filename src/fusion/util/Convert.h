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
 * @file   Convert.h
 * @author Chuck Sugnet
 * @date   Wed May  4 14:53:03 2005
 * 
 * @brief  Utility functions for converting between types.
 */

#ifndef _UTIL_CONVERT_H_
#define _UTIL_CONVERT_H_

//
#include "portability/affy-base-types.h"
//
#include <cmath>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
//

/**
 *  Convert
 * @brief Utility functions to convert between different types.
 */
class Convert {
  
 public:
  
  /** 
   * Make an int from a c string or die trying.
   * @param num - c string representation of number to be parsed.
   * 
   * @return - Integer representation of num
   */
  static int toInt(const std::string& num);

  /** 
   * Make an int from a c string.
   * @param num - c string representation of number to be parsed.
   * @param success - If success != NULL set to true upon 
   *   successful conversion and false upon failur.
   * @return - Integer representation of num
   */
  static int toIntCheck(const std::string& num, bool *success);

  /** 
   * Make an unsigned int from a c string or die trying.
   * @param num - c string representation of number to be parsed.
   * 
   * @return - Unsigned Integer representation of num
   */
  static unsigned int toUnsignedInt(const std::string& num);

  /** 
   * Make an unsigned int from a c string.
   * @param num - c string representation of number to be parsed.
   * @param success - If success != NULL set to true upon 
   *   successful conversion and false upon failur.
   * @return - Unsigned Integer representation of num, 0 if success == false
   */
  static unsigned int toUnsignedIntCheck(const std::string& num, bool *success);

  /** 
   * Make an unsigned int from a c string or die trying.
   * @param num - c string representation of number to be parsed.
   * 
   * @return - Unsigned Integer representation of num
   */
  static uint64_t toUnsignedInt64(const std::string& num);

  /** 
   * Make an unsigned int from a c string.
   * @param num - c string representation of number to be parsed.
   * @param success - If success != NULL set to true upon 
   *   successful conversion and false upon failur.
   * @return - Unsigned Integer representation of num, 0 if success == false
   */
  static uint64_t toUnsignedInt64Check(const std::string& num, bool *success);

  /** 
   * Make an float from a c string.
   * @param num - c string representation of number to be parsed.
   * 
   * @return - Float representation of num
   */
  static float toFloat(const std::string& num);
  
  /** 
   * Make an float from a c string.
   * @param num - c string representation of number to be parsed.
   * @param success - If success != NULL set to true upon 
   *   successful conversion and false upon failur.
   * @return - Float representation of num
   */
  static float toFloatCheck(const std::string& num, bool *success);

  /** 
   * Make an double from a c string.
   * @param num - c string representation of number to be parsed.
   * 
   * @return - Double representation of num
   */
  static double toDouble(const std::string& num);

  /** 
   * Make an double from a c string.
   * @param num - c string representation of number to be parsed.
   * @param success - If success != NULL set to true upon 
   *   successful conversion and false upon failur.
   * @return - Double representation of num
   */
  static double toDoubleCheck(const std::string& num, bool *success);

  /** 
   * Make an bool from a c string.
   * @param flag - c string representation of number to be parsed. 
   *   allowed value are 'true' && 'false'
   * 
   * @return - Bool representation of flag
   */
  static bool toBool(const std::string& flag);

  /** 
   * Make an bool from a c string.
   * @param flag - c string representation of number to be parsed.
   *   allowed value are 'true' && 'false'
   * @param success - If success != NULL set to true upon 
   *   successful conversion and false upon failur.
   * @return - Bool representation of flag, false on failure.
   */
  static bool toBoolCheck(const std::string& flag, bool *success);


  /** 
   * Make a string from an integer.
   * @param i - integer to be converted.
   * 
   * @return - String version of int i;
   */
  static std::string toString(int i);

  /** 
   * Make a string from an double
   * @param d - double to be converted.
   * 
   * @return - String version of int i;
   */
  static std::string toString(double d);

  /** 
   * Test to make sure that two doubles are pretty similar.
   * @param d1 
   * @param d2 
   * @param digits - number of decimal places to examine.
   * @return true if doubles are same to digits decimal places.
   */
  static bool doubleCloseEnough(double d1, double d2, int digits=6);

  /** 
   * Make a low precision float from a normal one. Used originally to
   * mimic truncation seen in cel files.
   * @param f float to be truncated.
   * @return truncated float
   */
  static float floatLowPrecision(float f);

  /**
   * Utility function to cut a delimited string into a vector of ints.
   *
   * @param s - delimited string.
   * @param delim - delimiter to chop on.
   * @param vec - vector to be filled in.
   */
  static void strToIntVec(const std::string &s,char delim,std::vector<int> &vec);

  /**
   * Utility function to cut a delimited string into a vector of strings.
   *
   * @param s - delimited string.
   * @param delim - delimiter to chop on.
   * @param vec - vector to be filled in.
   */
  static void strToStrVec(const std::string& s, const char delim, std::vector<std::string>& vec);

  /**
   * The inverse of strToIntVec
   *
   * @param inputVector - the vector to convert
   * @param delim - delimiter to use
   * @return - the resulting string
   */
  static std::string intVecToString(const std::vector<int> &inputVector, const std::string &delim);

  /**
   * Same as intVecToString but using string vectors
   *
   * @param inputVector - the vector to convert
   * @param delim - delimiter to use
   * @return - the resulting string
   */
  static std::string strVecToString(const std::vector<std::string> &inputVector, const std::string &delim);
};

/** Little template function to make string conversion easy. 
    this isn't the fastest way to do things, but it is easy. */
template <class T> 
std::string ToStr(const T &t) {
  std::ostringstream s;
  s.setf(std::ios_base::boolalpha);
  s << t;
  return s.str();
}

/// @brief     Convert the arg to a string.
/// @param     arg     which is a string already.
/// @return    a string
/// @remark    Since it is a string already, no conversion needs to be done.
///            ToStr returns a new string, so we match that.
// template <> 
inline std::string ToStr(const std::string& str) {
  std::string newstr=str;
  return newstr;
}

inline std::string ToStr(double t) {
  return Convert::toString(t);
}

inline std::string ToStr(float t) {
  return Convert::toString(static_cast<double>(t));
}

#endif /* _UTIL_CONVERT_H_ */

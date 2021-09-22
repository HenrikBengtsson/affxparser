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
 * @file   TableFile.h
 * @author Chuck Sugnet
 * @date   Mon Jun 27 19:55:06 2005
 * 
 * @brief  Class for dealing with column oriented files.
 */

#ifndef TABLEFILE_H
#define TABLEFILE_H

#include "util/RowFile.h"
#include "util/Util.h"
//
#include <climits>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>
//

/**
 *  TableFile - File for reading and using files that have delimited
 *                    rows of data. Specifically for R table files and the 
 *                    like. 
 */
class TableFile {

public: 

  /// Error value indicating not found.
  static const unsigned int npos = UINT_MAX;

  /** 
   * Constructor.
   * @param delim - Character to use for word separator. i.e. ',' or '\\t'
   * @param comment - Character that comment lines begin with.
   * @param useRowNames - Is first column the names of the rows?
   * @param useColNames - Is the first row the column headers?
   */
  TableFile(char delim = '\t', char comment = '#', bool useRowNames = false, bool useColNames = true);

  /** 
   * Destructor.
   */
  ~TableFile();

  /** 
   * Read data from file specified.
   * @param fileName - file to be read.
   * @return - true if successful.
   */
  bool open(const std::string& fileName);

  /** 
   * Read data from file specified.
   * @param fileName - file to be read.
   * @param requiredCols - column names that must be present.
   * @return - true if successful.
   */
  bool open(const std::string& fileName, const std::vector<std::string> &requiredCols);

  /** 
   * Write data to file specified.
   * @param fileName - name of file to write to.
   * @return - true if successful.
   */
  bool write(const std::string& fileName);

  /** 
   * Return number of columns in data.
   * @return - number of columns.
   */
  size_t numCols() { return m_ColNames.size(); }

  /** 
   * Return number of rows in data.
   * @return - number of rows.
   */
  size_t numRows() { return m_RowNames.size(); }

  /** 
   * Look up the index of column with name specified.
   * @param colName - Name of column of interest.
   * @return - npos if not found, column index otherwise.
   */
  unsigned int colIndex(const std::string& colName);

  /** 
   * Look up the index of row with name specified.
   * @param rowName - Name of row of interest.
   * @return - npos if not found, row index otherwise.
   */
  unsigned int rowIndex(const std::string& rowName);

  /** 
   * Return data at the position specified in table.
   * @param rowIx - row number.
   * @param colIx - column number.
   * @return - string representation of data.
   */
  std::string &getData(unsigned int rowIx, unsigned int colIx) { 
    return m_Data.at(rowIx).at(colIx); 
  }

  /** 
   * Return column name at specified index.
   * @param colIx - column number of interest.
   * @return - reference to column name at index of interest.
   */
  std::string &getColName(unsigned int colIx) { return m_ColNames.at(colIx); }

  /** 
   * Return row name at specified index.
   * @param rowIx - Row number of interest.
   * @return - reference to row name at index of interest.
   */
  std::string &getRowName(size_t rowIx) { return m_RowNames.at(rowIx); }

  /** 
   * Write out words in vector delimited by specified character.
   * @param out - stream to write to.
   * @param data - vector of data to write.
   * @param delim - delimiter to separate words with.
   */
  static void writeVector(std::ostream &out, std::vector<std::string> data, char delim);

  /** 
   * Rip the column out of a file by name filling in the data in colVec.
   * 
   * @param fileName - File to read column from.
   * @param colVec - Strings to be filled in from column.
   * @param colName - Name of column to be read in.
   * @param skipCols - Number of (non-commented) rows to skip before header.
   * @param unique - If true check to make sure every entry is unique.
   * 
   * @return true if successful false otherwise.
   */
  static bool columnFromFile(const std::string& fileName,
                             std::vector<std::string> &colVec, 
                             const std::string& colName,
                             unsigned int skipCols, bool unique=false);

  /** 
   * Rip the column out of a file by index filling in the data in colVec.
   * 
   * @param fileName - File to read column from.
   * @param colVec - Strings to be filled in from column.
   * @param colIx - Column index to read.
   * @param skipCols - Number of (non-commented) rows to skip before header.
   * @param unique - If true check to make sure every entry is unique.
   * 
   * @return true if successful false otherwise.
   */
  static bool columnFromFile(const std::string& fileName,
                             std::vector<std::string> &colVec, 
                             unsigned int colIx, unsigned int skipCols, bool unique=false);
  
  /** 
   * Rip a column out of an open RowFile.
   * 
   * @param rf - RowFile to read from.
   * @param colVec - Strings to be filled in with column's data.
   * @param colIx - Column index to read.
   * @param unique - If true check to make sure every entry is unique.
   * 
   * @return true if successful false otherwise.
   */
  static bool columnFromRowFile(RowFile &rf, std::vector<std::string> &colVec, 
                                unsigned int colIx, bool unique=false);
  /** 
   * @brief Change state to use row names.
   */  
  inline void setUseRowNames() { m_UseRowNames = true; }

  /** 
   * @brief Are we using row names?
   * @return true if using row names, false otherwise.
   */
  inline bool getUseRowNames() { return m_UseRowNames; }

  /** 
   * Set the flag to look for column names (i.e. column headers).
   * @param use - should column names be used
   */  
  inline void setUseColNames(bool use) { m_UseColNames = use; }

  /** 
   * Set the flag to look for row names (i.e. row ids).
   * @param use - should row names be used
   */  
  inline void setUseRowNames(bool use) { m_UseRowNames = use; }

  /** 
   * @brief Get the value of a layout key=value header pair.
   * @param key String to lookup value of.
   * 
   * @return value of key/value pair.
   */
  const std::vector<std::string> &getHeaderValue(const std::string &key) { 
    std::map<std::string, std::vector<std::string> >::iterator iter;
    iter = m_Header.find(key);
    if(iter == m_Header.end())
      Err::errAbort("Don't recognize header key: " + key);
    return iter->second;
  }

private:
  
  char m_Delim;              ///< Delimiter in table.
  char m_Comment;            ///< Comment character for data.
  bool m_UseRowNames;        ///< Should row names be expected
  bool m_UseColNames;        ///< Should row names be expected
  ///< Header parameters from %thisKey=thisParam
  std::map<std::string, std::vector<std::string> > m_Header;    
  ///< All header lines for easy recovery.    
  std::vector<std::string> m_HeaderLines;   
  ///< Name of columns.      
  std::vector<std::string> m_ColNames;      
 ///< Name of rows.
  std::vector<std::string> m_RowNames;           
  ///< Core table of words.
  std::vector< std::vector<std::string> > m_Data;      

  /** Iterator to walk through the map. */
  typedef std::map<std::string, unsigned int, Util::ltstring>::iterator TMapIter;
  /** Iterator to walk through the map. */
  typedef const std::map<std::string, unsigned int, Util::ltstring>::iterator TMapConstIter;

  /** Used as hash of column names. */
  std::map<std::string, unsigned int, Util::ltstring> m_ColNameMap;
  /** Used as hash of row names. */
  std::map<std::string, unsigned int, Util::ltstring> m_RowNameMap;
  /** Way to quickly free up memory in hash. */
  std::vector<char *> m_MemToFree; 
};

#endif /* TABLEFILE_H */

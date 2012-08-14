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
 * @file   TableFile.cpp
 * @author Chuck Sugnet
 * @date   Tue Jun 28 07:45:07 2005
 * 
 * @brief  Class for dealing with column oriented files.
 */

#include "util/TableFile.h"
//
#include "util/Convert.h"
#include "util/Err.h"
#include "util/Fs.h"
#include "util/RowFile.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <iostream>
//

using namespace std;

/** 
 * Constructor.
 * @param delim - Character to use for word separator. i.e. ',' or '\\t'
 * @param comment - Character that comment lines begin with.
 */
TableFile::TableFile(char delim, char comment, bool useRowNames, bool useColNames) 
  : m_Delim(delim), m_Comment(comment), m_UseRowNames(useRowNames), m_UseColNames(useColNames) {
}

/** 
 * Destructor.
 */
TableFile::~TableFile() {
  unsigned int i;
  for(i = 0; i < m_MemToFree.size(); i++) {
    char *p = m_MemToFree[i];
    delete [] p;
  }
}

/** 
 * Look up the index of column with name specified.
 * @param colName - Name of column of interest.
 * @return - npos if not found, column index otherwise.
 */
unsigned int TableFile::colIndex(const std::string& colName) {
  assert(colName!="");
  TMapConstIter iter = m_ColNameMap.find(colName);
  if(iter == m_ColNameMap.end())
    return npos;

  return iter->second;
}

/** 
 * Look up the index of row with name specified.
 * @param rowName - Name of row of interest.
 * @return - npos if not found, row index otherwise.
 */
unsigned int TableFile::rowIndex(const std::string& rowName) {
  assert(rowName!="");
  TMapConstIter iter = m_RowNameMap.find(rowName);
  if(iter == m_RowNameMap.end())
    return npos;

  return iter->second;
}
    
/** 
 * Read data from file specified.
 * @param fileName - file to be read.
 * @return - true if successful.
 */
bool TableFile::open(const std::string& fileName) {
  std::vector<string> requiredCols;
  return open(fileName, requiredCols);
}

/** 
 * Read data from file specified.
 * @param fileName - file to be read.
 * @param requiredCols - column names that must be present.
 * @return - true if successful.
 */
bool TableFile::open(const std::string& fileName, const std::vector<std::string> &requiredCols) {
  assert(fileName!="");
  vector<bool> found(requiredCols.size());
  vector<string> words;
  char *name = NULL;
  int rowOffset = 0;
  unsigned int i = 0;
  int count = 0;
  RowFile rf;

  rowOffset = (m_UseRowNames == true) ? 1 : 0;

  std::string tmp_unc_path=Fs::convertToUncPath(fileName);
  rf.open(tmp_unc_path.c_str());

  /* Read in any header. */
  rf.readHeader(rf, m_Header, m_HeaderLines);

  /* Read in column names. */
  if(!(rf.nextRow(words)) || words.empty())
    Err::errAbort("Nothing after header in file: "+FS_QUOTE_PATH(tmp_unc_path));

  if(m_UseColNames) {
    /* Read in the column header names. */
    for(i = 0; i < words.size(); i++) {
      name = Util::cloneString(words[i].c_str());
      //m_MemToFree.push_back(name);
      
      /* Was this a required column? */
      for(unsigned int requiredIx = 0; requiredIx < requiredCols.size(); requiredIx++) {
        if(requiredCols[requiredIx] == name) {
          found[requiredIx] = true;
        }
      }
      if(m_ColNameMap.find(name) != m_ColNameMap.end())
        Verbose::out(1, "Warning: Duplicate name: " + words[i] + 
                     " in column headers from file: " + Fs::basename(fileName));
      m_ColNameMap[name] = m_ColNames.size();
      m_ColNames.push_back(words[i]);
    }
    /* Check to make sure we found the required column names. */
    for(unsigned int requiredIx = 0; requiredIx < requiredCols.size(); requiredIx++) {
      if(found[requiredIx] == false) {
        Err::errAbort("Didn't find required column name: '" + 
                        requiredCols[requiredIx] + "' in file: " + fileName);
      }
    }
  }
  else {
    rf.reuseLine();
  }
  /* Read in each row. */
  while(rf.nextRow(words)) {
    string nameSpoof;
    // Spoof the column names if not being read.
    if(!m_UseColNames && m_Data.empty()) {
      unsigned int colIx = 0;
      if(m_UseRowNames)
        colIx = 1;
      for(colIx = colIx; colIx < words.size(); colIx++) {
        m_ColNames.push_back(ToStr(colIx));
      }
    }
    if(words.size() - rowOffset != m_ColNames.size()) 
      Err::errAbort("Expecting " + ToStr(m_ColNames.size()) + " words but got " +
                      ToStr(words.size() - rowOffset) + " at line " + 
                      ToStr(rf.getCurrentLineNumber()));

    assert(words.size() > 0);
    if(m_UseRowNames) {
      name = Util::cloneString(words[0].c_str());
    }
    else {
      nameSpoof = Convert::toString(count);
      name = Util::cloneString(nameSpoof.c_str());
    }
    m_MemToFree.push_back(name);      
    if(m_RowNameMap.find(name) != m_RowNameMap.end())
      Err::errAbort("Duplicate name: " + words[0] + " in row names.");
    m_RowNameMap[name] = m_Data.size();
    if(m_UseRowNames) {
      m_RowNames.push_back(words[0]);
      words.erase(words.begin());
    }
    else {
      m_RowNames.push_back(nameSpoof);
    }
    m_Data.push_back( words  );
    count++;
  }
  rf.close();
  return true;
}

/** 
 * Write out words in vector delimited by specified character.
 * @param out - stream to write to.
 * @param data - vector of data to write.
 * @param delim - delimiter to separate words with.
 */
void TableFile::writeVector(std::ostream &out, std::vector<std::string> data, char delim) {
  unsigned int i = 0;
  for( i = 0; i < data.size() - 1; i++) {
    out << data[i];
    out.put(delim);
  }
  if(data.size() > 0)
    out << data[data.size()-1];
  out << endl;
}
                 
/** 
 * Write data to file specified.
 * @param fileName - name of file to write to.
 * @return - true if successful.
 */
bool TableFile::write(const std::string& fileName) {
  assert(fileName!="");
  if(m_Data.empty()) 
    Err::errAbort("TableFile::write() - No data do write.");
  ofstream out(fileName.c_str());
  unsigned int i = 0;
  RowFile::writeHeader(out, m_HeaderLines);
  writeVector(out, m_ColNames, m_Delim);
  for(i = 0; i < m_Data.size(); i++) {
    out << m_RowNames[i];
    out.put(m_Delim);
    writeVector(out, m_Data[i], m_Delim);
  }
  return true;
}

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
bool TableFile::columnFromFile(const std::string& fileName,
                               std::vector<std::string> &colVec, 
                               const std::string& colName, unsigned int skipCols, bool unique) {
  RowFile rf;
  vector<string> words;
  rf.open(fileName.c_str());
  unsigned int colIx = npos;

  /* Skip rows. */
  while(skipCols > 0 && rf.nextRow(words)) {
    skipCols--;
  }
  /* Make sure there is something after the skipped entries. */
  if(!(rf.nextRow(words)) || words.empty())
    Err::errAbort("Nothing after header in file: " + string(fileName));
  for(unsigned int i = 0; i < words.size(); i++) {
    if(words[i] == colName) {
      if(colIx == npos) {
        colIx = i;
      }
      else {
        Verbose::out(1, "Warning: column name: " + ToStr(colName) + 
                " occurs multiple times in: " + ToStr(fileName) + 
                " using first column.");
      }
    }
  }
  /* Did we find a column with that name? */
  if(colIx == npos)
    return false;
  return columnFromRowFile(rf, colVec, colIx, unique);
}

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
bool TableFile::columnFromFile(const std::string& fileName,
                               std::vector<std::string> &colVec, 
                               unsigned int colIx, unsigned int skipCols, bool unique) {
  RowFile rf;
  vector<string> words;
  rf.open(fileName.c_str());

  /* Skip rows. */
  while(skipCols > 0 && rf.nextRow(words)) {
    skipCols--;
  }
  /* Make sure there is something there. */
  if(!(rf.nextRow(words)) || words.empty())
    Err::errAbort("Nothing after header in file: " + string(fileName));
  rf.reuseLine();
  return columnFromRowFile(rf, colVec, colIx, unique);
}

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
bool TableFile::columnFromRowFile(RowFile &rf, std::vector<std::string> &colVec, 
                               unsigned int colIx, bool unique) {
  map<string,bool> seen;
  vector<string> words;
  string fileName = rf.getFileName();
  /* Read entries. */
  while(rf.nextRow(words)) {
    if(colIx >= words.size()) {
      Err::errAbort("Trying to read column: " + ToStr(colIx) + " from row with only: " + 
                      ToStr(words.size()) + " columns at line: " + ToStr(rf.getCurrentLineNumber()) +
                      " in file: " + fileName);
    }
    if(unique) {
      if(seen.find(words[colIx]) != seen.end()) {
        Err::errAbort("Entry: '" + words[colIx] + "' has already been seen in file: " + 
                        fileName + " in column " + ToStr(colIx) + ".");
      }
      else {
        seen[words[colIx]] = true;
      }
    }
    colVec.push_back(words[colIx]);
  }
  return true;
}

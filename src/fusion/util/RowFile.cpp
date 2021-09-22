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
 * @file   RowFile.cpp
 * @author Chuck Sugnet
 * @date   Mon May 23 11:26:39 2005
 * @brief  Class for dealing with line oriented files.
 */

//
#include "util/RowFile.h"
//
#include "util/Convert.h"
#include "util/Err.h"
#include "util/Fs.h"
#include "util/Util.h"
//
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std; 

/**
 * Constructor. Open an object with delimiter and comment
 * characters. 
 * @param delim - word delimiter character.
 * @param comment - line comment character. 
 */
RowFile::RowFile(char delimChar, char commentChar, char commentChar2):
   m_Buffer(NULL), m_BufferSize(0), m_LineIx(0),
  m_Delim(delimChar), m_Comment(commentChar), m_Comment2(commentChar2), 
  m_Reuse(false) {
}

/**
 * Open file fileName or die trying.
 * @param char * fileName - full path of fileName to be opened.
 */
void RowFile::open(const std::string& fileName) {

  m_FileName = fileName;
  Fs::aptOpen( m_In, fileName, ios_base::in);
  if(m_In.fail()) {
    Err::errAbort("Can't open file "+FS_QUOTE_PATH(fileName) +" to read.");
  }
  m_EndType = determineLineEndingType(m_In);
}

/** 
 * Destructor. 
 */
RowFile::~RowFile() { 
  if(m_Buffer) {
    delete[] m_Buffer;
  } 
  if(m_In.is_open()) { 
    m_In.close(); 
  }
};

/**
 * Close file streams. 
 */
void RowFile::close() {
  m_In.close();
}

/**
 * Pull the next line from the input m_Buffer or die trying.
 *
 * @return - Pointer to string representation of current line or NULL
 * at EOF.
 */
const string *RowFile::nextLine() {
  
  /* If we're reusing this line just return it again. */
  if(m_Reuse) {
    m_Reuse = false;
    return &m_CurrentLine;
  }
  /* if end of file return NULL. */
  if(m_In.eof())
    return NULL;
  /* Check stream state. */
  if(m_In.fail())
    Err::errAbort("Something bad happened before line: " + 
		    ToStr(m_LineIx));

  /* Little logic to handle the different line endings possible. */
  if(m_EndType == MAC) {
    getline(m_In, m_CurrentLine, '\r');
  }
  else { 
    getline(m_In, m_CurrentLine, '\n');
    // if ms-dos chop off the '\r'
    if(m_EndType == DOS && m_CurrentLine.length() > 0) 
      m_CurrentLine.erase(m_CurrentLine.end() - 1);
  }
  Util::trimString(m_CurrentLine);
  m_LineIx++;
  return &m_CurrentLine;
}

/**
 * Return the next line that is non-blank and doesn't start with a
 * comment character.
 * @return - Pointer to string repesentation of current line or NULL
 * at EOF.
 */
const string *RowFile::nextRealLine() {
  unsigned int notWhiteSpace = 0;
  while( nextLine() != NULL) {
    notWhiteSpace = 0;
    // Skip the first not white space.
    while(notWhiteSpace < m_CurrentLine.size() &&
          isspace(m_CurrentLine[notWhiteSpace]))
      notWhiteSpace++;
    // If found we found a line with characters that doesn't start
    // with a comment return true.
    if(m_CurrentLine.size() != 0 && 
       m_CurrentLine[notWhiteSpace] != m_Comment && 
       m_CurrentLine[notWhiteSpace] != m_Comment2)
      return &m_CurrentLine;
  }
  // never found a real line.
  return NULL;
}

/**
 * Chop the next row into words and return them in the vector.
 * @param words - vector to fill in with words.
 * @return bool - false at EOF true otherwise.
 */
bool RowFile::nextRow(std::vector<std::string> &words) {
  int len = 0, start = 0, next = 0;

  if(nextRealLine() == NULL)
    return false;
  words.clear();
  len = m_CurrentLine.size();

  while(start < len) {
    next = (int)m_CurrentLine.find(m_Delim, start);
    if(next < 0) {
      next = m_CurrentLine.size(); // entire string.
    }
    words.push_back(m_CurrentLine.substr(start, next - start));
    start = next+1;
  }
  return true;
}

/**
 * Chop the next row into words and return them in the
 * vector. char* memory in vector is owned elsewhere and will
 * change after an calls to nextRow or nextLine.
 * @param words - vector to fill in with words.  @return bool -
 * false at EOF true otherwise.
 */
bool RowFile::nextCStringRowExpect(std::vector<const char *> &words, unsigned int expect) {
  if(!nextCStringRow(words))
    return false;
  if(words.size() != expect) {
    Err::errAbort("Got " + ToStr(words.size()) +
		    " words instead of " + ToStr(expect) +
		    " expected at line: " + ToStr(m_LineIx));
  }
  return true;
}

/**
 * Chop the next row into words and return them in the
 * vector. char* memory in vector is owned elsewhere and will
 * change after an calls to nextRow or nextLine.
 * @param words - vector to fill in with words.  @return bool -
 * false at EOF true otherwise.
 */
bool RowFile::nextCStringRow(std::vector<const char *> &words) {
  int len = 0, start = 0, next = 0;
  char *mark = 0;
  char *buff = 0;

  if(nextRealLine() == NULL)
    return false;
  words.clear();
  len = m_CurrentLine.size();

  /* Use the m_Buffer to copy strings into to avoid lots of new's and
     deletes. Only expand when necessary. */
  if(len + 1 > m_BufferSize) {
    FreezArray(m_Buffer);
    m_BufferSize = len+1;
     m_Buffer = new char[m_BufferSize];
  }
  //strncpy(m_Buffer, m_CurrentLine.c_str(), len);
  memcpy(m_Buffer, m_CurrentLine.c_str(), len);
  buff = m_Buffer;
  
  /* Trim off any delimiters at beginning and end of line */
  while(start < len) {
    if(!isspace(m_CurrentLine[start])) 
      break;
    start++;
  }
  while(len > start) {
    if(!isspace(m_CurrentLine[len -1]))
      break;
    m_Buffer[len - 1] = '\0';
    len--;
  }

  /* Loop through and cut up lines into words based on delimiter. */
  while(start < len) {
    next = (int)m_CurrentLine.find(m_Delim, start);
    if(next < 0) {
      next = m_CurrentLine.size(); // entire string.
    }
    mark = buff+next;
    *mark = '\0';
    words.push_back(buff+start);
    start = next+1;
  }
  return true;
}

/**
 * Chop the next row into words and return them in the vector
 * squawk and die if expected number was different from number
 * found.
 * @param words - vector to fill in with words.
 * @param expect - number of words that should be in next row.
 * @return bool - false at EOF true otherwise.
 */
bool RowFile::nextRowExpect(std::vector<std::string> &words, unsigned int expect) {
  if(!nextRow(words))
    return false;
  if(words.size() != expect) {
    Err::errAbort("Got " + ToStr(words.size()) +
		    " words instead of " + ToStr(expect) +
		    " expected at line: " + ToStr(m_LineIx));
  }
  return true;
}
  
void RowFile::readHeader(RowFile &rf, std::map<std::string, std::vector<std::string> > &header,
                         std::vector<std::string> &lines) {
  char delim = '=';
  int startPos = 0;
  string::size_type delimPos = 0;
  const string *line = NULL;
  while((line = rf.nextLine()) != NULL) {
    startPos = 0;
    /* Header is prefixed by '%' or "#%" */
    if(line->length() == 0)
      continue;
    else if(line->length() > 1 && (*line)[0] == rf.m_Comment && (*line)[1] == rf.m_Comment2)
      startPos = 2;
    else if(line->length() > 0 && (*line)[0] == rf.m_Comment2)
      startPos = 1;
    else {
      rf.reuseLine();
      break;
    }    
    lines.push_back(string(*line));
    delimPos = line->find(delim);
    if(delimPos == string::npos) 
      Err::errAbort("Couldn't find delimiter: '" + ToStr(delim) + 
                      "' in line:\n" + string(*line));
    string key = line->substr(startPos, delimPos-startPos);
    string value = line->substr(delimPos+1, line->length());
    if(header.find(key) == header.end()) {
      // first time seen, create a new vector of values.
      vector<string> vals;
      vals.push_back(value);
      header[key] = vals;
    }
    else {
      vector<string> &vals = header[key];
      vals.push_back(value);
    }
  }   
} 

/**
 * Write out a header in #%key=value form.
 */
void RowFile::writeHeader(std::ostream &out, const std::vector<std::string> &lines) {
  typedef vector<string>::const_iterator vecI;
  vecI i;
  for(i = lines.begin(); i != lines.end(); ++i) 
    out << *i << endl;
}

/** 
 * Read in a matrix from a file. 
 * @param fileName - Name of file to read from.
 * @param matrix - where to read data into.
 * @param skipLines - lines (possibly header?) to skip.
 * @param skipCols - number of columns (row names?) to skip.
 */
void RowFile::matrixFromFile(const std::string& fileName,
                             std::vector< std::vector<double> > &matrix,
                             unsigned int skipLines, unsigned int skipCols) {
  RowFile rf;
  unsigned int i = 0;
  vector<string> words;
  rf.open(fileName.c_str());
  for(i = 0; i < skipLines; i++)
    rf.nextRow(words);
  while(rf.nextRow(words)) {
    if(skipCols >= words.size()) 
      Err::errAbort("RowFile::matrixFromFile() - Number of skipCols >= number of cols.");
    matrix.push_back(vector<double>());
    for(i = skipCols; i < words.size(); i++) {
      matrix[matrix.size() -1].push_back(Convert::toDouble(words[i].c_str()));
    }
  }
}

/** 
 * Read in a matrix from a file. 
 * @param fileName - Name of file to read from.
 * @param matrix - where to read data into.
 * @param skipLines - lines (possibly header?) to skip.
 * @param skipCols - number of columns (row names?) to skip.
 */
void RowFile::matrixFromFile(const std::string& fileName,
                             std::vector< std::vector<float> > &matrix,
                             unsigned int skipLines, unsigned int skipCols) {
  RowFile rf;
  unsigned int i = 0;
  vector<string> words;
  rf.open(fileName.c_str());
  for(i = 0; i < skipLines; i++)
    rf.nextRow(words);
  while(rf.nextRow(words)) {
    if(skipCols >= words.size()) 
      Err::errAbort("RowFile::matrixFromFile() - Number of skipCols >= number of cols.");
    matrix.push_back(vector<float>());
    for(i = skipCols; i < words.size(); i++) {
      matrix[matrix.size() -1].push_back(Convert::toFloat(words[i].c_str()));
    }
  }
}


/** 
 * What type of line endings do we see in this input stream? 
 * @param input - stream to look for '\r','\n', or '\r\n' in.
 * @return type of line endings we expect in the future.
 */
enum RowFile::FileLineEnding RowFile::determineLineEndingType(std::ifstream &input) {
  FileLineEnding ending = UNKNOWN;
  while(input.good()) {
    char c1 = input.get();
    // '\r' alone indicates mac, '\r\n' indicates dos
    if(c1 == '\r') {
      if(input.good() && input.get() == '\n') {
        ending = DOS;
      }
      else {
        ending = MAC;
      }
      break;
    }
    // is it a unix file?
    else if(c1 == '\n') {
      ending = UNIX;
      break;
    }
  }
  // reset stream back to beginning of file.
  input.seekg(0);
  return ending;
}

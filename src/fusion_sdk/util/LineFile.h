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
 * @file   LineFile.h
 * @author Mybrid Spalding
 * @date   Mon May 23 11:27:16 2005
 *
 * @brief  DO NOT USE. Except in cases of test code. Platform safe file class to read lines.
 */

#ifndef _LINE_FILE_H_
#define _LINE_FILE_H_

//
#include <fstream>
#include <string>
//

/**
 *  LineFile
 * @brief Platform safe class for reading lines.
 *
 * For Test Code ONLY.
 * Chances are you should be using file/TsvFile/TsvFile
 * and not this object. This file is intended for test code in util/
 * being used by file/TsvFile/TsvFile and other
 * code not using TsvFile.
 */

class LineFile
{

public:

  /** Line endings are platform specific, this encodes different types
      we know of. */
  enum FileLineEnding {
    UNIX = 0,   // '\n'
    DOS = 1,    // '\r\n'
    MAC = 2,    // '\r'
    UNKNOWN = 3 // ???
  };

  /**
   * Constructor.
   */
  LineFile();

  /**
   * @brief Destructor closes stream.
   */
  virtual ~LineFile();

  /**
   * @brief Platform safe open for read, can be used multiple times
   * with different file names.
   * @return ifstream in question.
   *         
   */
  std::ifstream & open(const std::string & fileName, bool abortOnError = false);
  /**
   * @brief close the ifstream if needed. 
   * @return void
   */
  void            close();

  /**
   * @brief Platform safe reading of a line, stripping the eol.
   * File is automatically closed when eof is reached.
   * @param line - returned if possible, cleared otherwise.
   * @param trim - false by default, true strips trailing spaces.
   * @return true if line is returned.
   *         line will be cleared when false is returned. .
   */
  bool getline(std::string & line, bool trim = false);

  /**
   * @brief What type of file is this one?
   * @return Type of file that has been determined.
   */
  enum FileLineEnding getFileType() {
    return m_endType;
  }

  /**
   * @brief What is the name of the file we're reading from?
   * @return name of file being read.
   */
  std::string getFileName() {
    return m_fileName;
  }

  /**
   * @brief - get the input file stream.
   * @return ifstream reference.
   */
  std::ifstream & getStream() {
    return m_fileStream;
  }

  bool is_open() {
    return m_fileStream.is_open();
  }
  bool fail() {
    return m_fileStream.fail();
  }
  bool eof() {
    return m_fileStream.eof() ;
  }

private:

  std::string m_fileName;
  std::ifstream m_fileStream;
  enum FileLineEnding m_endType;


};

#endif /* _LINE_FILE_ */

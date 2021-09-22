////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011 Affymetrix, Inc.
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
 * @file   LineFile.cpp
 * @author Mybrid Spalding
 * @date   Wed Feb  2 10:04:48 PST 2011
 * @brief  Test class for reading files by line that is platform safe.
 */

//
#include "util/LineFile.h"
//
#include "util/Err.h"
#include "util/Fs.h"
#include "util/Util.h"
//

/**
 * What type of line endings do we see in this input stream?
 * @param input - stream to look for '\r','\n', or '\r\n' in.
 * @return type of line endings we expect in the future.
 */
static enum LineFile::FileLineEnding _determineLineEndingType(std::ifstream & fileStream)
{
  enum LineFile::FileLineEnding ending = LineFile::UNKNOWN;
  while(fileStream.good()) {
    char c1 = fileStream.get();
    // '\r' alone indicates mac, '\r\n' indicates dos
    if(c1 == '\r') {
      if(fileStream.good() && fileStream.get() == '\n') {
        ending = LineFile::DOS;
      } else {
        ending = LineFile::MAC;
      }
      break;
    }
    // is it a unix file?
    else if(c1 == '\n') {
      ending = LineFile::UNIX;
      break;
    }
  }
  // reset stream back to beginning of file.
  fileStream.seekg(0);
  return ending;
}

/**
 * Constructor.
 */
LineFile::LineFile(): m_endType(UNKNOWN) {}

/**
 * Destructor.
 */
LineFile::~LineFile()
{
  if(m_fileStream.is_open()) {
    m_fileStream.close();
  }
};

/**
 * Open file fileName or die trying.
 * @param char * fileName - full path of fileName to be opened.
 */
std::ifstream & LineFile::open(const std::string& fileName, bool abortOnError)
{

  if(m_fileStream.is_open()) {
    m_fileStream.close();
  }

  m_fileName = fileName;

  Fs::aptOpen(m_fileStream, m_fileName, std::ios_base::in);
  if(!m_fileStream.is_open()) {
    if(abortOnError) {
      Err::errAbort("Can't open file " + FS_QUOTE_PATH(fileName) + " to read.");
    }
  } else {
    m_endType = _determineLineEndingType(m_fileStream);
  }

  return m_fileStream;
}


/**
 * Close file streams.
 */
void LineFile::close()
{
  if(m_fileStream.is_open()) {
    m_fileStream.close();
  }
}

/**
 * LineFile::getline
 *
 * Read the next line if the stream is open.
 * When eof is reached, close the file.
 * @return -
 * at EOF.
 */
bool LineFile::getline(std::string & line, bool trim)
{

  line.clear();

  if(!m_fileStream.is_open() || m_fileStream.fail() || m_fileStream.eof()) {
    return false;
  }


  if(m_endType == MAC) {
    std::getline(m_fileStream, line, '\r');
  } else {
    std::getline(m_fileStream, line, '\n');
    // if ms-dos chop off the '\r'
    if(m_endType == DOS && line.length() > 0) {
      line.erase(line.end() - 1);
    }
  }

  if(trim && (line.length() > 0)) {
    Util::trimString(line);
  }

  return true;
}


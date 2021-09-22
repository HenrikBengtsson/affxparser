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
 * @file   RowFile.h
 * @author Chuck Sugnet
 * @date   Mon May 23 11:27:16 2005
 * 
 * @brief  Class for dealing with line oriented files.
 */

#ifndef ROWFILE_H
#define ROWFILE_H

#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>
//

/**
 *  RowFile
 * @brief Class for reading line oriented files and parsing them
 * out into vectors of strings easily.
 */
class RowFile {

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
    * Constructor. Open an object with delimiter and comment
    * characters. 
    * @param delimChar - word delimiter character.
    * @param commentChar - line comment character. 
    * @param commentChar2 - second line comment character. 
    */
   RowFile(char delimChar = '\t', char commentChar = '#', char commentChar2 = '%');

   /** 
    * @brief Destructor.
    */
  ~RowFile();
   
  /**
   * Open file fileName or die trying.
   * @param fileName - full path of fileName to be opened.
   */
  void open(const std::string& fileName);
  
  /**
   * Close file streams. 
   */
  void close();
  
  /**
   * Chop the next row into words and return them in the vector
   * squawk and die if expected number was different from number
   * found.
   * @param words - vector to fill in with words.
   * @param expect - number of words that should be in next row.
   * @return bool - false at EOF true otherwise.
   */
  bool nextRowExpect(std::vector<std::string> &words, unsigned int expect);

  /**
   * Chop the next row into words and return them in the vector
   * squawk and die if expected number was different from number
   * found. char * memory in vector is owned elsewhere and will
   * change after an calls to nextRow or nextLine.
   * @param words - vector to fill in with words.
   * @param expect - number of words that should be in next row.
   * @return bool - false at EOF true otherwise.
   */
  bool nextCStringRowExpect(std::vector<const char *> &words, unsigned int expect);
    
   /**
    * Chop the next row into words and return them in the
    * vector. char* memory in vector is owned elsewhere and will
    * change after an calls to nextRow or nextLine.
    * @param words - vector to fill in with words.  @return bool -
    * false at EOF true otherwise.
    */
  bool nextCStringRow(std::vector<const char *> &words);
  /**
   * Chop the next row into words and return them in the vector.
   * @param words - vector to fill in with words.
   * @return bool - false at EOF true otherwise.
   */
  bool nextRow(std::vector<std::string> &words);
  
  /**
   * Set the delimiter.
   * @param nDelim - new word delimiter.
   */
  void setDelim(char nDelim) { m_Delim = nDelim; }

  /**
   * Get the delimiter.
   * @return current word delimiter. 
   */
  char getDelim() { return m_Delim; }

  /**
   * Set the comment.
   * @param nComment - new comment character.
   */
  void setComment(char nComment) { m_Comment = nComment; }

  /**
   * Get the comment.
   * @return current comment character. 
   */
  char getComment() { return m_Comment; }

  /**
   * Pull the next line from the input buffer or die trying.
   */
  const std::string *nextLine();

  /**
   * Return the next line that is non-blank and doesn't start with a
   * comment character.
   */
  const std::string *nextRealLine();

  /**
   * Reuse the same line again.
   */
  void reuseLine() { m_Reuse = true; }

  /** 
   * @brief What line number are we reading?
   * @return Line number.
   */
  int getCurrentLineNumber() { return m_LineIx; }

  /**
   * Fill in a header map with data from the top of the file. 
   */
  static void readHeader(RowFile &rf, std::map<std::string, std::vector<std::string> > &header, 
                         std::vector<std::string> &lines);

  /**
   * Write out a header in #%key=value form.
   */
  static void writeHeader(std::ostream &out, const std::vector<std::string> &lines);

  /** 
   * Read in a matrix from a file. 
   * @param fileName - Name of file to read from.
   * @param matrix - where to read data into.
   * @param skipLines - lines (possibly header?) to skip.
   * @param skipCols - number of columns (row names?) to skip.
   */
  static void matrixFromFile(const std::string& fileName,
                             std::vector< std::vector<double> > &matrix,
                             unsigned int skipLines=0, unsigned int skipCols=0);

  /** 
   * Read in a matrix from a file. 
   * @param fileName - Name of file to read from.
   * @param matrix - where to read data into.
   * @param skipLines - lines (possibly header?) to skip.
   * @param skipCols - number of columns (row names?) to skip.
   */
  static void matrixFromFile(const std::string& fileName,
                             std::vector< std::vector<float> > &matrix,
                             unsigned int skipLines=0, unsigned int skipCols=0);
  
  /** 
   * @brief What type of file is this one?
   * @return Type of file that has been determined.
   */
  inline enum FileLineEnding getFileType() { return m_EndType; }

  /**
   * What is the name of the rowfile we're reading from?
   * @return name of file being read.
   */
  std::string getFileName() { return m_FileName; }

private:

  /** 
   * What type of line endings do we see in this input stream? 
   * @param input - stream to look for '\r','\n', or '\r\n' in.
   * @return type of line endings we expect in the future.
   */
  enum RowFile::FileLineEnding determineLineEndingType(std::ifstream &input);

  /// Name of file we are reading from.
  std::string m_FileName;
  /// Stream that we are reading from.
  std::ifstream m_In; 
  /// Current line read from the file.
  std::string m_CurrentLine;
  /// Utility buffer for chopping into char *s, faster than strings.
  char *m_Buffer;
  /// Current size of the buffer.
  int m_BufferSize;
  /// Line number in file that is currently read.
  int m_LineIx;
  /// Separating delimiter, usually tab ('\t') or comma (',')
  char m_Delim;
  /// Comment character usually '#'
  char m_Comment;
  /// Secondary comment character, usually '%'
  char m_Comment2;
  /// Should we reuse the last line again?
  bool m_Reuse;
  /// What type of line endings does this stream use?
  FileLineEnding m_EndType;
};

#endif /* ROWFILE_H */

////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


/// @file  TsvJoin.h
/// @brief Headers for TsvJoin.cpp.

#ifndef TSV_JOIN_H
#define TSV_JOIN_H

#include "file/TsvFile/TsvFile.h"
//
#include "util/Err.h"
#include "util/PgOptions.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <cassert>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>
//

class tsvJoin
{
public:
  /** Constructor.
   * @param argc Number of command line arguments.
   * @param argv Command line arguments.
   * @param version Version string.
   */

  tsvJoin (int argc,
           const char* argv[],
           const std::string& version);

  /** Destructor.
   */
  ~tsvJoin();

  /** Read, process data, write output.
   */
  void run();

private:

  /** Clear data.
   */
  void clear();

  /** Begin output.
   */
  void beginOutput (void);

  /** Open input files.
   */
  void openInputFiles (void);

  /** Begin output file header.
   */
  void beginOutputHeader (void);

  /** Write header lines copied from input file.
   * @param tsv Tsv object for file.
   * @param idx File index.
   * @param columnNames Column names.
   */
  void writeHeadersFromFile (affx::TsvFile& tsv, const unsigned int idx,
    std::vector<std::string>& columnNames);

  /** Write column names.
   */
  void writeColumnNames (void);

  /** Write matching lines.
   */
  void writeMatches (void);

  /** Write first part of file related header line.
   * @param i File index.
   */
  void headerLine (const unsigned int i);

  /// private data
  /// Version string.
  const std::string& m_Version;
  /// Command line options.
  PgOptions* m_Opts;
  /// Command line as a string.
  std::string m_CommandLine;
  /// Key.
  std::string m_Key;
  /// Output file name.
  std::string m_Outfile;
  /// Should filenames be prepended?
  bool m_Prepend;
  /// Number of input files.
  unsigned int m_FileCount;
  /// Input file names.
  std::vector<std::string> m_FileNames;
  /// Output file stream.
  std::ofstream m_Out;
  /// Stream mode input file tsv object.
  affx::TsvFile m_StreamTsv;
  /// Stream mode input file column names.
  std::vector<std::string> m_StreamColNames;
  /// Stream mode input file key value for row.
  std::string m_StreamKeyValue;
  /// Stream mode input file data values for row.
  std::vector<std::string> m_StreamData;
  /// Index mode input file tsv objects.
  std::vector<affx::TsvFile*> m_IndexTsvs;
  /// Index mode input file key values.
  std::vector<std::string> m_IndexKeyValues;
  /// Index mode input file column names.
  std::vector<std::vector<std::string> > m_IndexColNames;
  /// Index mode input file data values.
  std::vector<std::vector<std::string> > m_IndexData;
  /// Execution guid.
  std::string m_ExecGuid;
  /// Output section separator.
  const std::string m_CommentLine;
  /// Number of data columns in stream file.
  unsigned int m_StreamDataColCount;
  /// Number of data columns in index files.
  std::vector<unsigned int> m_IndexDataColCount;

};

#endif /* TSV_JOIN_H */

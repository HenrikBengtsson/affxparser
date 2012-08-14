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

#ifndef DUMP_PGF_H
#define DUMP_PGF_H

//
#include "file/TsvFile/SequentialClfFile.h"
#include "file/TsvFile/TsvFile.h"
//
#include "util/Err.h"
#include "util/PgOptions.h"
#include "util/Util.h"
//
#include <cassert>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>
//

/**
 * @file  DumpPgf.h
 * @brief Headers for DumpPgf.cpp.
 */
class dumpPgf
{
public:
  /** Constructor.
   * @param argc Number of command line arguments.
   * @param argv Command line arguments.
   * @param version Version string.
   */

  dumpPgf (const char* argv[],
           const std::string& version);

  /** Destructor.
   */
  ~dumpPgf()
  {
    clear();
  }

  /** Read, process data, write output.
   */
  void run();

private:

  /** Clear data.
   */
  void clear()
  {
    //delete m_Opts;
  }

  /** Read optional transcript cluster id and probeset id files.
   */
  void readIdFiles (void);

  /** Begin output.
   */
  void beginOutput (void);

  /** Open input files.
   */
  void openInputFiles (void);

  /** Write output file header.
   */
  void writeOutputHeader (void);

  /** Write lines matching selection criteria.
   */
  void writeMatches (void);

  /** Dump information for a given probeset id.
   */
  void dumpProbesetData (void);

  /** Dump information for a given probe id.
   */
  void dumpProbeData (void);

  /** Split type by '->'.
   */
  void splitTypes (const std::string& inputString, std::vector<std::string>& inputTypes);

  /** Calculate probe coordinates using SequentialClfFile.
   *
   * @param probeId Probe id.
   */
  void getCoordsBySequentialClf (const int probeId);

  /** Calculate probe coordinates using index.
   *
   * @param probeId Probe id.
   */
  void getCoordsByIndex (const int probeId);

  /// private data
  /// Version string.
  const std::string& m_Version;
  /// Command line options.
public:
  PgOptions m_Opts;
private:
  /// Command line as a string.
  std::string m_CommandLine;
  /// Pgf file name.
  std::string m_PgfFileName;
  /// Clf file name.
  std::string m_ClfFileName;
  /// Probeset type(s).
  std::vector<std::string> m_ProbesetTypes;
  /// Probeset id file name(s).
  std::vector<std::string> m_ProbesetIdFileNames;
  /// Probe id file name(s).
  std::vector<std::string> m_ProbeIdFileNames;
  /// Should probeset level information only be dumped?
  bool m_DumpProbesetsOnly;
  /// Should the union of the types requested be dumped?
  bool m_DumpUnion;
  /// Pgf file tsv object.
  affx::TsvFile m_PgfTsv;
  /// Clf file tsv object.
  affx::TsvFile m_ClfTsv;
  /// Sequential clf file object.
  SequentialClfFile m_SequentialClf;
  /// Column names for output.
  std::vector<std::string> m_OutputColNames;
  /// Number of probeset (level 0) data columns.
  unsigned int m_ProbesetDataCount;
  /// Number of atom (level 1) data columns.
  unsigned int m_AtomDataCount;
  /// Number of probe (level 2) data columns.
  unsigned int m_ProbeDataCount;
  /// Probeset (level 0) data, excluding probeset_id.
  std::vector<std::string> m_ProbesetData;
  /// Atom and probe (levels 1 and 2) data, excluding probe_id.
  std::vector<std::string> m_AtomProbeData;
  /// Execution guid.
  std::string m_ExecGuid;
  /// Probeset ids requested.
  std::vector<int> m_ProbesetIds;
  /// Probe ids requested.
  std::vector<int> m_ProbeIds;
  /// Pgf file probeset_id.
  int m_PgfProbesetId;
  /// Pgf file probe_id.
  int m_PgfProbeId;
  /// Column containing probeset type.
  unsigned int m_ProbesetTypeCol;
  /// Clf file probe_id.
  int m_ClfProbeId;
  /// Clf file x value.
  int m_ClfX;
  /// Clf file y value.
  int m_ClfY;
  /// Output file name.
  std::string m_Outfile;
  /// Output stream.
  std::ostream* m_Out;
  /// File output stream.
  std::ofstream m_FileOut;
  // Follow Stroustrup third edition section 15.5 'pointers to members'.
  typedef void (dumpPgf::* PMemProbeCoord_t) (const int probeId);
  // Pointer to function to determine the probe coordinates.
  PMemProbeCoord_t m_GetProbeCoordinates;
};

#endif /* DUMP_PGF_H */

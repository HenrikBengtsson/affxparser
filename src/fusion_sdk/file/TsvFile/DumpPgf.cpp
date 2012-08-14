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

/// @file   DumpPgf.cpp
/// @brief  Class for dumping information from a pgf and clf file.

#include "file/TsvFile/DumpPgf.h"
//
#include "util/Fs.h"
#include "util/Guid.h"
#include "util/Util.h"
//
#include <algorithm>
//

using namespace std;
using namespace affx;

void define_dumppgf_options(PgOptions* opts) {
  opts->setUsage("apt-dump-pgf - Dump information from a pgf file.\n"
                 "Usage:\n"
                 "   apt-dump-pgf -o int.txt -c file.clf -p file.pgf [--probeset-type=type [--probeset-type=...]]\n\n"
                 "   apt-dump-pgf -o int.txt -c file.clf -p file.pgf [--probeset-ids=file [--probeset-ids=...]]\n\n"
                 "   apt-dump-pgf -o int.txt -c file.clf -p file.pgf [--probe-ids=file [--probe-ids=...]]"
                 );
  opts->defineOption("p", "pgf-file", PgOpt::STRING_OPT,
                     "The pgf file used to dump information.",
                     "");
  opts->defineOption("c", "clf-file", PgOpt::STRING_OPT,
                     "Optional clf file to use. When present, "
                     "probe position will be included in the output.",
                     "");
  opts->defineOption("", "probeset-only", PgOpt::BOOL_OPT,
                     "Dump only probeset level information.",
                     "false");
  opts->defOptMult("", "probeset-type", PgOpt::STRING_OPT,
                   "Optional probeset type to extract; "
                   "can be specified multiple times. When specified "
                   "multiple times, the intersection of all types "
                   "is taken. The user cannot mix use of probeset-type, "
                   "probeset-ids, and probe-ids.",
                   "");
  opts->defineOption("s", "probeset-ids", PgOpt::STRING_OPT,
                     "Optional name of a file containing probeset ids "
                     "to extract; can be specified multiple times. The "
                     "user cannot mix use of probeset-type, probeset-ids, "
                     "and probe-ids.",
                     "");
  opts->defineOption("", "probe-ids", PgOpt::STRING_OPT,
                     "Optional name of a file containing probe ids "
                     "to extract; can be specified multiple times. The "
                     "user cannot mix use of probeset-type, probeset-ids, "
                     "and probe-ids.",
                     "");
  opts->defineOption("", "or", PgOpt::BOOL_OPT,
                     "Use the union of the types requested, not the intersection.",
                     "false");
  opts->defineOption("o", "out-file", PgOpt::STRING_OPT,
                     "Output file to contain the dump output.",
                     "");
  opts->defineOption("", "version", PgOpt::BOOL_OPT,
                     "Display version information.",
                     "false");
  opts->defineOption("h", "help", PgOpt::BOOL_OPT,
                     "Print help message.",
                     "false");
}

/**
 *  @brief Constructor.
 *
 *  @param argc Number of command line arguments.
 *  @param argv Command line arguments.
 *  @param version Version string.
 *
 *  Errors: throw exception to display help messages, if unsupported
 *  option choices were made.
*/
dumpPgf::dumpPgf (const char* argv[], 
                  const std::string& version)
  : m_Version (version), m_GetProbeCoordinates (0)
{
  // Prefer throw() to exit().
  Err::setThrowStatus (true);

  //
  //m_Opts = new PgOptions ();
  define_dumppgf_options(&m_Opts);
  m_Opts.parseArgv(argv);

  // The command line is copied to output.
  m_CommandLine=m_Opts.commandLine();

  // Optionally display usage message.
  if (m_Opts.getBool("help") || (m_Opts.argc() <= 1))
  {
    m_Opts.usage();
    cout << "version: " << version << "\n";
    exit(0);
  }
  // Optionally display version.
  if (m_Opts.getBool("version"))
  {
    cout << "version: " << version << "\n";
    exit(0);
  }

  // Require pgf file.
  m_PgfFileName = m_Opts.get("pgf-file");
  if (m_PgfFileName.empty())
  {
    string msg = "Must provide pgf file.";
    Err::errAbort (msg);
  }
  // Optional clf file.
  m_ClfFileName = m_Opts.get("clf-file");

  PgOpt* opt;

  // Save optional types.
  opt = m_Opts.mustFindOpt("probeset-type");
  opt->push_user_values_into(m_ProbesetTypes);
  // Save optional probeset
  opt = m_Opts.mustFindOpt("probeset-ids");
  opt->push_user_values_into(m_ProbesetIdFileNames);
  // and probe id file names.
  opt = m_Opts.mustFindOpt("probe-ids");
  opt->push_user_values_into(m_ProbeIdFileNames);

  // Allow only one of probeset-type, probeset-ids, or probe-ids options.
  int optionsChosen = 0;
  optionsChosen += m_ProbesetTypes.empty() ? 0 : 1;
  optionsChosen += m_ProbesetIdFileNames.empty() ? 0 : 1;
  optionsChosen += m_ProbeIdFileNames.empty() ? 0 : 1;

  if (optionsChosen > 1)
  {
    string msg = "Cannot mix use of --probeset-ids, --probe-ids, ";
    msg += "and --probeset-type.";
    Err::errAbort (msg);
  }

  // Save optional probeset-only, or flags.
  m_DumpProbesetsOnly = m_Opts.getBool("probeset-only");
  m_DumpUnion = m_Opts.getBool("or");

  // Probeset-only is incompatible with a probe-ids list.
  if (m_DumpProbesetsOnly && !m_ProbeIdFileNames.empty())
    Err::errAbort ("Cannot use --probeset-only with --probe-ids.");

  // Output to cout unless a writeable file was selected.
  m_Outfile = m_Opts.get("out-file");
  if (m_Outfile.empty())
    Err::errAbort ("Must provide an output file, --out-file option.");
  else
  {
    Fs::mustOpenToWrite(m_FileOut, m_Outfile);
    m_Out = &m_FileOut;
  }


}

/**
 *  @brief Read, process data, write output.
 *
 *  Errors: abort if unable to read any input file.
*/
void dumpPgf::run()
{
  // Write initial output.
  beginOutput();

  // Read optional probeset or probe id files.
  readIdFiles();

  // Open input files.
  openInputFiles();

  // Write output header.
  writeOutputHeader();

  // Write lines matching selection criteria.
  writeMatches();
}

/**
 *  @brief Read optional probeset or probe id files.
*/
void dumpPgf::readIdFiles()
{
  // Probeset id file(s).
  map <int, bool> probesetIdsMap;
  for (unsigned int i = 0; i < m_ProbesetIdFileNames.size(); ++i)
  {
    string& probesetIdFileName = m_ProbesetIdFileNames[i];
    TsvFile tsv;
    int probesetId;
    tsv.bind (0, "probeset_id", &probesetId, TSV_BIND_REQUIRED);
    if (tsv.open (probesetIdFileName) != TSV_OK)
      Err::errAbort ("Problem opening probeset id file " + probesetIdFileName);
    while (tsv.nextLevel (0) == TSV_OK)
      // Ignore duplicate ids.
      if (probesetIdsMap.insert (make_pair (probesetId, true)) .second)
        m_ProbesetIds.push_back (probesetId);
    tsv.close();
  }
  Verbose::out(1,"Found " + ToStr(m_ProbesetIds.size()) + " probesets in probeset list files.");
  // Probe id file(s).
  map <int, bool> probeIdsMap;
  for (unsigned int i = 0; i < m_ProbeIdFileNames.size(); ++i)
  {
    string& probeIdFileName = m_ProbeIdFileNames[i];
    TsvFile tsv;
    int probeId;
    tsv.bind (0, "probe_id", &probeId, TSV_BIND_REQUIRED);
    if (tsv.open (probeIdFileName) != TSV_OK)
      Err::errAbort ("Problem opening probe id file " + probeIdFileName);
    while (tsv.nextLevel (0) == TSV_OK)
      // Ignore duplicate ids.
      if (probeIdsMap.insert (make_pair (probeId, true)) .second)
        m_ProbeIds.push_back (probeId);
    tsv.close();
  }
  Verbose::out(1,"Found " + ToStr(m_ProbeIds.size()) + " probes in probe list files.");
}

/**
 *  @brief Begin output.
*/
void dumpPgf::beginOutput()
{
    Verbose::out(1,"MODULE: " + m_Version);
    Verbose::out(1,"CMD: " + m_CommandLine);
    m_ExecGuid = affxutil::Guid::GenerateNewGuid();
    Verbose::out(1,"exec_guid " + m_ExecGuid);
}

/**
 *  @brief Open input pgf, clf files.
 *
 *  Errors: abort if a file could not be opened or if
 *  a required column name was not found.
*/
void dumpPgf::openInputFiles()
{
    Verbose::out(1,"Reading meta data from PGF and CLF files");
  // Open pgf file.
  m_PgfTsv.bind (0, "probeset_id", &m_PgfProbesetId, TSV_BIND_REQUIRED);
  if (! m_DumpProbesetsOnly)
    m_PgfTsv.bind (2, "probe_id", &m_PgfProbeId, TSV_BIND_REQUIRED);
  if (m_PgfTsv.open (m_PgfFileName) != TSV_OK)
    Err::errAbort ("Problem opening file " + m_PgfFileName + ".");

  // If a clf file name was provided, open it.
  if (! m_ClfFileName.empty())
  {
    if (m_ClfTsv.open (m_ClfFileName) != TSV_OK)
      Err::errAbort ("Problem opening file " + m_ClfFileName + ".");
    if (m_SequentialClf.isSequential (m_ClfTsv))
      // If sequential, calculate coordinates by simple arithmetic.
      m_GetProbeCoordinates = &dumpPgf::getCoordsBySequentialClf;
    else
    {
      // Not sequential, use index to find coordinates.
      // Require probe_id, x, y columns.
      m_ClfTsv.bind (0, "probe_id", &m_ClfProbeId, TSV_BIND_REQUIRED);
      m_ClfTsv.bind (0, "x", &m_ClfX, TSV_BIND_REQUIRED);
      m_ClfTsv.bind (0, "y", &m_ClfY, TSV_BIND_REQUIRED);
      // Will be indexing over probe_id.
      m_ClfTsv.defineIndex (0, "probe_id", TSV_INDEX_INT, 0);
      m_GetProbeCoordinates = &dumpPgf::getCoordsByIndex;
    }
  }

  // If probeset-ids file(s) were provided, index over probeset_id.
  if (! m_ProbesetIdFileNames.empty())
    m_PgfTsv.defineIndex (0, "probeset_id", TSV_INDEX_INT, 0);
  // Likewise for probe ids - already required that the user did not request both.
  if (! m_ProbeIdFileNames.empty())
    m_PgfTsv.defineIndex (2, "probe_id", TSV_INDEX_INT, 0);

  // Set up output header, bind pgf file data.
  m_ProbesetDataCount = m_PgfTsv.getColumnCount(0);
  // Save probeset level data other than the probeset_id (int)
  // as strings.
  m_ProbesetData.resize (m_ProbesetDataCount - 1);
  unsigned int k = 0;
  bool foundType = false;
  for (unsigned int i = 0; i < m_ProbesetDataCount; ++i)
  {
    string colName;
    m_PgfTsv.cidx2cname (0, i, colName);
    m_OutputColNames.push_back (colName);
    // Probeset_id is an int, type and probeset_name are strings.
    // To make generating output simpler, require that the
    // first column is the probeset_id.
    assert (! ((i == 0) && (colName != "probeset_id")) );
    if (colName != "probeset_id")
    {
      if (colName == "type")
      {
	m_ProbesetTypeCol = k;
	foundType = true;
      }
      m_PgfTsv.bind (0, i, &m_ProbesetData[k++]);
    }
  }
  // Subtract probeset_id column from data count.
  --m_ProbesetDataCount;
  // Require type column if user requested -probeset-type.
  if (! m_ProbesetTypes.empty() && ! foundType)
    Err::errAbort ("No type column in pgf file");
  // Bind atom, probe level data unless -probeset-only selected.
  if (m_DumpProbesetsOnly)
    m_AtomDataCount = m_ProbeDataCount = 0;
  else
  {
    m_AtomDataCount = m_PgfTsv.getColumnCount(1);
    m_ProbeDataCount = m_PgfTsv.getColumnCount(2);
    // Save atom and probe level data other than the probe_id (int)
    // as strings.
    m_AtomProbeData.resize (m_AtomDataCount + m_ProbeDataCount - 1);
    k = 0;
    for (unsigned int i = 0; i < m_AtomDataCount; ++i)
    {
      string colName;
      m_PgfTsv.cidx2cname (1, i, colName);
      m_OutputColNames.push_back (colName);
      m_PgfTsv.bind (1, i, &m_AtomProbeData[k++]);
    }
    for (unsigned int i = 0; i < m_ProbeDataCount; ++i)
    {
      string colName;
      m_PgfTsv.cidx2cname (2, i, colName);
      m_OutputColNames.push_back (colName);
      // Again, to make generating output simpler, require that
      // the first probe level column is the probe_id.
      assert (! ((i == 0) && (colName != "probe_id")) );
      // Already bound probe_id.
      if (colName != "probe_id")
	m_PgfTsv.bind (2, i, &m_AtomProbeData[k++]);
    }
    // Subtract probe_id column from data count.
    --m_ProbeDataCount;
    // If using a clf file, will write x and y columns.
    if (! m_ClfFileName.empty())
    {
      m_OutputColNames.push_back ("x");
      m_OutputColNames.push_back ("y");
    }
  } // end if (m_DumpProbesetsOnly)
}

/**
 *  @brief Write output file header.
*/
void dumpPgf::writeOutputHeader()
{
  // Generic apt meta tags.
  const string guid = affxutil::Guid::GenerateNewGuid();
  *m_Out << "#%guid=" << guid << "\n";
  *m_Out << "#%exec_guid=" << m_ExecGuid << "\n";
  *m_Out << "#%exec_version=" << m_Version << "\n";
  *m_Out << "#%create_date=" << Util::getTimeStamp() << "\n";
  *m_Out << "#%cmd=" << m_CommandLine << "\n";

  // Copy genome position file header meta tags to output.
  string key, value;
  m_PgfTsv.headersBegin();
  while (m_PgfTsv.headersNext (key, value) == TSV_OK)
    // Propagate only chip_type, lib_set_version, and lib_set_name.
    if ( (key == "chip_type") || (key == "lib_set_version") ||
	(key == "lib_set_name") )
      *m_Out << "#%" << key << "=" << value << "\n";

  // Write header line.
  unsigned int colNameCount = m_OutputColNames.size();
  assert (colNameCount > 0);
  *m_Out << m_OutputColNames[0];
  for (unsigned int i = 1; i < colNameCount; ++i)
    *m_Out << "\t" << m_OutputColNames[i];
  *m_Out << "\n";
}

/**
 *  @brief Write lines matching selection criteria.
*/
void dumpPgf::writeMatches()
{
  // If using a clf file, need a method for obtaining probe coordinates.
  if (! m_ClfFileName.empty())
    assert (m_GetProbeCoordinates);

  // Indexing probesets.
  if (! m_ProbesetIdFileNames.empty())
  {
    const unsigned int probesetIdCount = m_ProbesetIds.size();
    for (unsigned int i = 0; i < probesetIdCount; ++i)
    {
      if(i==0)
          Verbose::out(1,"Indexing probesets in PGF file");
      const int probesetId = m_ProbesetIds[i];
      if (m_PgfTsv.findBegin (0, "probeset_id", TSV_OP_EQ, probesetId) != TSV_OK)
	Err::errAbort ("Problem reading pgf file " + m_PgfFileName);
      const int resultCount = m_PgfTsv.findResultsCount();
      // Skip if no match.
      if (resultCount == 0)
        continue;

      // Fatal error if more than one match was found.
      else if (resultCount > 1)
      {
        string msg = "probeset_id '" + ToStr (probesetId);
        msg += "' is not a unique index. Duplicate probeset_id found, [";
        msg += ToStr (probesetId) + "] for pgf file " + m_PgfFileName;
        Err::errAbort (msg);
      }
      // Found one match - read, write data.
      if (m_PgfTsv.findNext() != TSV_OK)
        Err::errAbort ("Problem reading pgf file " + m_PgfFileName);
      if(i==0)
        Verbose::out(1,"Dumping probeset info");
      dumpProbesetData();
    }
  }

  // Indexing probes.
  else if (! m_ProbeIdFileNames.empty())
  {
    const unsigned int probeIdCount = m_ProbeIds.size();
    for (unsigned int i = 0; i < probeIdCount; ++i)
    {
      const int probeId = m_ProbeIds[i];
      if(i==0)
        Verbose::out(1,"Indexing probes in PGF file");
      if (m_PgfTsv.findBegin (2, "probe_id", TSV_OP_EQ, probeId) != TSV_OK)
        Err::errAbort ("Problem reading pgf file " + m_PgfFileName);
      const int resultCount = m_PgfTsv.findResultsCount();
      // Skip if no match.
      if (resultCount == 0)
        continue;

      // Fatal error if more than one match was found.
      else if (resultCount > 1)
      {
        string msg = "probe_id '" + ToStr (probeId);
        msg += "' is not a unique index. Duplicate probe_id found, [";
        msg += ToStr (probeId) + "] for pgf file " + m_PgfFileName;
        Err::errAbort (msg);
      }
      // Found one match - read, write data.
      if (m_PgfTsv.findNext() != TSV_OK)
        Err::errAbort ("Problem reading pgf file " + m_PgfFileName);
      if(i==0)
        Verbose::out(1,"Dumping probe info");
      dumpProbeData();
    }
  }

  // Dumping by probeset type.
  else if (! m_ProbesetTypes.empty())
  {
    Verbose::out(1,"Scanning PGF file probe for requested type(s)");
    const vector<string>::const_iterator userTypesBegin = m_ProbesetTypes.begin();
    const vector<string>::const_iterator userTypesEnd = m_ProbesetTypes.end();
    const unsigned int userTypesCount = m_ProbesetTypes.size();
    // Types in line read from pgf file.
    vector<string> pgfFileTypes;
    // User entered -or.
    if (m_DumpUnion)
      while (m_PgfTsv.nextLevel (0) == TSV_OK)
      {
	// Split type by '->'.
	splitTypes (m_ProbesetData[m_ProbesetTypeCol], pgfFileTypes);
	// Dump this probeset_id if pgf file type matches any of those specified by the user.
	const unsigned int pgfFileTypesCount = pgfFileTypes.size();
	for (unsigned int i = 0; i < pgfFileTypesCount; ++i)
	  if (find (userTypesBegin, userTypesEnd, pgfFileTypes[i]) != userTypesEnd)
	  {
	    dumpProbesetData();
	    break;
	  }
      } // end while (m_PgfTsv.nextLevel (0) == TSV_OK)
    // Default 'and' - require pgf file types to include all of those specified by the user.
    else
      while (m_PgfTsv.nextLevel (0) == TSV_OK)
      {
	splitTypes (m_ProbesetData[m_ProbesetTypeCol], pgfFileTypes);
	const vector<string>::const_iterator pgfFileTypesBegin = pgfFileTypes.begin();
	const vector<string>::const_iterator pgfFileTypesEnd = pgfFileTypes.end();
	bool noMismatchFound = true;
	// Dump this probeset_id if type matches all of those specified by the user.
	for (unsigned int i = 0; i < userTypesCount; ++i)
	{
	  if (find (pgfFileTypesBegin, pgfFileTypesEnd, m_ProbesetTypes[i]) == pgfFileTypesEnd)
	  {
	    // If one of the user requested types is not present in this pgf file line,
	    // don't write the line.
	    noMismatchFound = false;
	    break;
	  }
	}
	if (noMismatchFound)
	  dumpProbesetData();
      } // end while (m_PgfTsv.nextLevel (0) == TSV_OK)
  }

  // None of the above - dump the entire pgf file.
  else
  {
    Verbose::out(1,"Dumping entire PGF file");
    while (m_PgfTsv.nextLevel (0) == TSV_OK)
      dumpProbesetData();
  }
}

/**
 *  @brief Split type by '->'.
*/
void dumpPgf::splitTypes (const std::string& inputString, std::vector<std::string>& inputTypes)
{
  // Want to reuse the output vector; clear it first.
  inputTypes.clear();
  size_t substrBegin = 0;
  for (;;)
  {
    size_t substrEnd = inputString.find ("->", substrBegin);
    if (substrEnd == string::npos)
    {
      // No more '->' - save what's left, quit.
      string subString = inputString.substr (substrBegin);
      // Avoid returning a null string from a terminating '->' or an empty inputString.
      if (! subString.empty())
	inputTypes.push_back (subString);
      break;
    }
    // Avoid null strings from an initial '->' or '->->'.
    if (substrEnd != substrBegin)
      inputTypes.push_back (inputString.substr (substrBegin, substrEnd - substrBegin) );
    // Continue following the '->'
    substrBegin = substrEnd + 2;
  }
}

/**
 *  @brief Dump information for a given probeset id.
*/
void dumpPgf::dumpProbesetData()
{
  // If dumping probesets only, write out data.
  if (m_DumpProbesetsOnly)
  {
    *m_Out << m_PgfProbesetId;
    for (unsigned int j = 0; j < m_ProbesetDataCount; ++j)
      *m_Out << "\t" << m_ProbesetData[j];
    *m_Out << "\n";
    return;
  }
  // Read, dump atom, probe level data.
  while (m_PgfTsv.nextLevel (1) == TSV_OK)
    while (m_PgfTsv.nextLevel (2) == TSV_OK)
      dumpProbeData();
}

/**
 *  @brief Dump information for a given probe id.
*/
void dumpPgf::dumpProbeData()
{
  *m_Out << m_PgfProbesetId;
  for (unsigned int j = 0; j < m_ProbesetDataCount; ++j)
    *m_Out << "\t" << m_ProbesetData[j];
  // Keeping atom, probe level data, excluding probe_id, in
  // the m_AtomProbeData vector.
  unsigned int k = 0;
  for (unsigned int j = 0; j < m_AtomDataCount; ++j)
    *m_Out << "\t" << m_AtomProbeData[k++];
  // Previously required that the probe_id column be
  // first in the probe level data.
  *m_Out << "\t" << m_PgfProbeId;
  for (unsigned int j = 0; j < m_ProbeDataCount; ++j)
    *m_Out << "\t" << m_AtomProbeData[k++];
  // Dump x, y columns if a clf file is in use.
  if (! m_ClfFileName.empty())
  {
    (this->*m_GetProbeCoordinates) (m_PgfProbeId);
    *m_Out << "\t";
    // Write null coordinates if not defined.
    if (m_ClfX >= 0)
      *m_Out << m_ClfX;
    *m_Out << "\t";
    if (m_ClfY >= 0)
      *m_Out << m_ClfY;
  }
  *m_Out << "\n";
}

/**
 *  @brief Calculate probe coordinates using SequentialClfFile.
*/
void dumpPgf::getCoordsBySequentialClf (const int probeId)
{
  m_SequentialClf.getProbeCoordinates (probeId, m_ClfX, m_ClfY);
}

/**
 *  @brief Calculate probe coordinates using index.
*/
void dumpPgf::getCoordsByIndex (const int probeId)
{
  if (m_ClfTsv.findBegin (0, "probe_id", TSV_OP_EQ, m_PgfProbeId) != TSV_OK)
    Err::errAbort ("Problem reading clf file " + m_ClfFileName);
  const int resultCount = m_ClfTsv.findResultsCount();
  // Fatal error if more than one match was found.
  if (resultCount > 1)
  {
    string msg = "probe_id '" + ToStr (m_PgfProbeId);
    msg += "' is not a unique index. Duplicate probe_id found, [";
    msg += ToStr (m_PgfProbeId) + "] for clf file " + m_ClfFileName;
    Err::errAbort (msg);
  }
  else if (resultCount == 1)
  {
    // Found one match - read x, y.
    if (m_ClfTsv.findNext() != TSV_OK)
      Err::errAbort ("Problem reading clf file " + m_ClfFileName);
  }
  // Write null x, y coordinates if no match found.
  else
    m_ClfX = m_ClfY = -1;
}

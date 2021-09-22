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

/// @file   TsvJoin.cpp
/// @brief  Class for merging multiple tab separated text files.

//
#include "file/TsvFile/TsvJoin.h"
//
#include "util/Fs.h"
#include "util/Guid.h"
#include "util/Util.h"
//
#include <algorithm>
//
using namespace std;
using namespace affx;


void define_tsvjoin_options(PgOptions* opts)
{
  opts->setUsage("apt-tsv-join - Merge multiple tab separated text files into a single file.\n"
                 "Usage:\n"
                 "   apt-tsv-join -o data.txt -k column-name file1.txt file2.txt [...]");

  opts->defineOption("k", "key", PgOpt::STRING_OPT,
                     "Name of the column to use to join the various files.",
                     "");
  opts->defineOption("o", "out-file", PgOpt::STRING_OPT,
                     "Output file to send the merged results to.",
                     "");
  opts->defineOption("p", "prepend-filename", PgOpt::BOOL_OPT,
                     "Prepend the filename to the column header.",
                     "false");
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
 *  Errors: throw exception to display help messages, if too few input files.
*/
tsvJoin::tsvJoin (int argc,
                  const char* argv[],
                  const std::string& version)
  : m_Version (version),
  m_CommentLine ("############################################################\n")
{

  // Prefer throw() to exit().
  Err::setThrowStatus (true);

  m_Opts = new PgOptions();
  define_tsvjoin_options(m_Opts);
  m_Opts->parseArgv(argv);

  // Optionally display usage message.
  if (m_Opts->getBool("help") || argc == 1)
  {
    m_Opts->usage();
    string msg = "version: " + version + "\n";
    cout << msg;
    exit(0);
  }
  // Optionally display version.
  if (m_Opts->getBool("version"))
  {
    string msg = "version: " + version + "\n";
    cout << msg;
    exit(0);
  }

  // Require key.
  m_Key = m_Opts->get("key");
  if (m_Key.empty())
  {
    string msg = "FATAL: Must provide --key option.";
    Err::errAbort (msg);
  }
  // Require at least two input files.
  m_FileCount = (unsigned int)m_Opts->getArgCount();
  if (m_FileCount < 2)
  {
    string msg = "FATAL: Must provide 2 or more files to merge.";
    Err::errAbort (msg);
  }
  for (unsigned int i = 0; i < m_FileCount; ++i)
    m_FileNames.push_back (string (m_Opts->getArg(i)));

  // Require writeable output file.
  m_Outfile = m_Opts->get("out-file");
  if (m_Outfile.empty())
  {
    string msg = "FATAL: Must provide -out-file output file.";
    Err::errAbort (msg);
  }
  else
    Fs::mustOpenToWrite(m_Out, m_Outfile);

  // Save prepend status.
  m_Prepend = m_Opts->getBool("prepend-filename");

  // The command line is copied to the output file header.
  assert (argc > 0);
  m_CommandLine = argv [0];
  for (int i = 1; i < argc; ++i)
    m_CommandLine += " " + string (argv [i]);
}

/**
 *  @brief Read, process data, write output.
 *
 *  Errors: abort if unable to read any input file.
*/
void tsvJoin::run (void)
{
  // Write initial output.
  beginOutput();

  // Open input files.
  openInputFiles();

  // Generate initial output header lines.
  beginOutputHeader();

  // Write file specific header lines.
  unsigned int idx = 0;
  writeHeadersFromFile (m_StreamTsv, idx, m_StreamColNames);
  for (idx = 0; idx < m_FileCount - 1; ++idx)
    writeHeadersFromFile (*m_IndexTsvs[idx], idx + 1, m_IndexColNames[idx]);
  m_Out << m_CommentLine;

  // Write matching lines.
  writeMatches();

  // Close output file.
  m_Out.close();
}

/**
 *  @brief Begin output.
*/
void tsvJoin::beginOutput (void)
{
  Verbose::out (1, "MODULE: " + m_Version);
  Verbose::out (1, "CMD: " + m_CommandLine);
  m_ExecGuid = affxutil::Guid::GenerateNewGuid();
  Verbose::out (1, "exec_guid " + m_ExecGuid);
}

/**
 *  @brief Open input files.
 *
 *  Errors: abort if a file could not be opened or if
 *  the key column name was not found.
*/
void tsvJoin::openInputFiles (void)
{
  string& streamFileName = m_FileNames[0];
  Verbose::out (1, "Opening stream file " + streamFileName + ".");
  if (m_StreamTsv.open (streamFileName) != TSV_OK)
    Err::errAbort ("Problem opening file " + streamFileName);
  // Allow comment lines within data.
  m_StreamTsv.m_optAllowDataComment = true;

  bool foundKey = false;
  unsigned int colCount = m_StreamTsv.getColumnCount(0);
  m_StreamDataColCount = colCount - 1;
  m_StreamColNames.resize (colCount);

  for (unsigned int i = 0; i < colCount; ++i)
  {
    string colName;
    m_StreamTsv.cidx2cname (0, i, colName);
    m_StreamColNames[i] = colName;
    if (colName == m_Key)
    {
      m_StreamTsv.bind (0, i, &m_StreamKeyValue);
      foundKey = true;
    }
  }
  // Abort if key not found.
  if (! foundKey)
    Err::errAbort ("Key " + m_Key + " not found in file " + streamFileName);

  // Bind to data vector after checking column names - binds must follow
  // resize(), which may trigger reallocation, which would invalidate the
  // addresses passed in the bind.
  m_StreamData.resize (colCount - 1);
  unsigned int k = 0;
  for (unsigned int i = 0; i < colCount; ++i)
  {
    string colName;
    m_StreamTsv.cidx2cname (0, i, colName);
    if (colName != m_Key)
      m_StreamTsv.bind (0, i, &m_StreamData[k++]);
  }

  // Required in the constructor that at least two input file names were provided.
  assert (m_FileCount > 1);
  for (unsigned int i = 0; i < m_FileCount - 1; ++i)
  {
    string& indexFileName = m_FileNames[i + 1];
    Verbose::out (1, "Opening file " + indexFileName + " for indexing.");
    TsvFile* indexTsv = new TsvFile;
    if (indexTsv->open (indexFileName) != TSV_OK)
      Err::errAbort ("Problem opening file " + indexFileName);
    indexTsv->m_optAllowDataComment = true;
    m_IndexTsvs.push_back (indexTsv);

    colCount = indexTsv->getColumnCount(0);
    m_IndexColNames.push_back (vector<string> (colCount));
    m_IndexData.push_back (vector<string> (colCount - 1));

    foundKey = false;
    k = 0;
    for (unsigned int j = 0; j < colCount; ++j)
    {
      string colName;
      indexTsv->cidx2cname (0, j, colName);
      m_IndexColNames[i][j] = colName;
      if (colName == m_Key)
        foundKey = true;
    }
    if (! foundKey)
      Err::errAbort ("Key " + m_Key + " not found in file " + indexFileName);

    // Set up index.
    indexTsv->defineIndex (0, m_Key, TSV_INDEX_STRING, 0);

    // Remember the number of data columns.
    m_IndexDataColCount.push_back (colCount - 1);

  }  // end for (unsigned int i = 0; i < m_FileCount - 1; ++i)

  // Bind to the data vectors after setting up them up: push_back()
  // and resize() may cause reallocation, which will change the addresses.
  m_IndexKeyValues.resize (m_FileCount - 1);
  for (unsigned int i = 0; i < m_FileCount - 1; ++i)
  {
    TsvFile* indexTsv = m_IndexTsvs[i];
    k = 0;
    // Add one since had stored number of data columns.
    unsigned int colCount = m_IndexDataColCount[i] + 1;
    for (unsigned int j = 0; j < colCount; ++j)
    {
      string colName;
      indexTsv->cidx2cname (0, j, colName);
      if (colName == m_Key)
        indexTsv->bind (0, j, &m_IndexKeyValues[i]);
      else
        indexTsv->bind (0, j, &m_IndexData[i][k++]);
    }
  }
}

/**
 *  @brief Begin output file header.
*/
void tsvJoin::beginOutputHeader (void)
{
  const string guid = affxutil::Guid::GenerateNewGuid();
  m_Out << "#%guid=" << guid << "\n";
  m_Out << "#%exec_guid=" << m_ExecGuid << "\n";
  m_Out << "#%exec_version=" << m_Version << "\n";
  m_Out << "#%create_date=" << Util::getTimeStamp() << "\n";
  m_Out << "#%cmd=" << m_CommandLine << "\n";
}

/**
 *  @brief Write header lines copied from input file.
*/
void tsvJoin::writeHeadersFromFile (TsvFile& tsv, const unsigned int idx,
                                    std::vector<std::string>& columnNames)
{
  m_Out << m_CommentLine;
  headerLine (idx);
  m_Out << "name=" << m_FileNames[idx] << "\n";
  headerLine (idx);
  m_Out << "cols=";
  const unsigned int colCount = columnNames.size();
  // Need the key column at least.
  assert (colCount > 0);
  m_Out << columnNames[0];
  for (unsigned int i = 1; i < colCount; ++i)
    m_Out << "," << columnNames[i];
  m_Out << "\n";

  // Finished with the key column name - delete it.
  columnNames.erase (remove (columnNames.begin(), columnNames.end(), m_Key), columnNames.end());

  // Copy file header meta tags to output.
  string key, value;
  tsv.headersBegin();
  while (tsv.headersNext (key, value) == TSV_OK)
    // Comment lines among the headers give rise to a pair with a null key.
    if (! key.empty())
    {
      headerLine (idx);
      m_Out << key << "=" << value << "\n";
    }
}

/**
 *  @brief Write output column names.
*/
void tsvJoin::writeColumnNames (void)
{
  // Key first.
  m_Out << m_Key;

  // Remaining columns of the stream file.
  const unsigned int streamColCount = m_StreamColNames.size();
  for (unsigned int i = 0; i < streamColCount; ++i)
    // Write key only once.
    if (m_StreamColNames[i] != m_Key)
    {
      m_Out << "\t";
      // Prepend file name if requested.
      if (m_Prepend)
	m_Out << m_FileNames[0] << ": ";
      m_Out << m_StreamColNames[i];
    }

  // Remaining columns of index files.
  // The key column name has already been deleted, in writeHeadersFromFile().
  for (unsigned int idx = 0; idx < m_FileCount - 1; ++idx)
  {
    const unsigned int indexColCount = m_IndexColNames[idx].size();
    for (unsigned int i = 0; i < indexColCount; ++i)
    {
      m_Out << "\t";
      if (m_Prepend)
	m_Out << m_FileNames[idx + 1] << ": ";
      m_Out << m_IndexColNames[idx][i];
    }
  }
  m_Out << "\n";
}

/**
 *  @brief Write matching lines.
*/
void tsvJoin::writeMatches (void)
{
  Verbose::out (1, "Done indexing files. Generating merged file.");

  // Write column names only if find at least one match.
  bool columnNamesWritten = false;

  // Read stream file.
  while (m_StreamTsv.nextLevel (0) == TSV_OK)
  {
    bool foundMatch = false;
    // Look for matches in each index file.
    for (unsigned int idx = 0; idx < m_FileCount - 1; ++idx)
    {
      TsvFile* indexTsv = m_IndexTsvs[idx];
      if (indexTsv->findBegin (0, m_Key, TSV_OP_EQ, m_StreamKeyValue) != TSV_OK)
	Err::errAbort ("Problem reading index file " + m_FileNames[idx + 1]);

      const int resultCount = indexTsv->findResultsCount();
      // Require a match in all index files.
      if (resultCount == 0)
      {
        foundMatch = false;
	break;
      }
      // Fatal error if more than one match was found.
      else if (resultCount > 1)
      {
	string msg = "FATAL: key '" + m_Key + "' is not a unique index. Duplicate key found, [";
	msg += m_StreamKeyValue + "] for file " + m_FileNames[idx + 1] + ".";
	Err::errAbort (msg);
      }
      // Found one match - read data
      if (indexTsv->findNext() != TSV_OK)
	Err::errAbort ("Problem reading index file " + m_FileNames[idx + 1]);
      foundMatch = true;
    } // end for (unsigned int idx = 0; idx < m_FileCount - 1; ++idx)

    if (foundMatch)
    {
      // Found match - write column names once.
      if (! columnNamesWritten)
      {
	writeColumnNames();
	columnNamesWritten = true;
      }
      // Write key.
      m_Out << m_StreamKeyValue;

      // Write columns from stream file.
      for (unsigned int i = 0; i < m_StreamDataColCount; ++i)
	m_Out << "\t" << m_StreamData[i];

      // Write columns from index files.
      for (unsigned int idx = 0; idx < m_FileCount - 1; ++idx)
      {
	for (unsigned int i = 0; i < m_IndexDataColCount[idx]; ++i)
	  m_Out << "\t" << m_IndexData[idx][i];
      }
      m_Out << "\n";
    } // end if (foundMatch)
  } // end while (m_StreamTsv.nextLevel (0) == TSV_OK)
}

/** Destructor.
 */
tsvJoin::~tsvJoin()
{
  clear();
}

/** Clear data.
 */
void tsvJoin::clear()
{
  delete m_Opts;
  for (unsigned int i = 0; i < m_IndexTsvs.size(); ++i)
    {
      m_IndexTsvs[i]->close();
      delete m_IndexTsvs[i];
    }
}

void tsvJoin::headerLine (const unsigned int i)
{
  m_Out << "#%file" << i << "_";
}

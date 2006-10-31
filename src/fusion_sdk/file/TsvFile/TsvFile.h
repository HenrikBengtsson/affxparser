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
/// @file   TsvFile.h
/// @brief  Headers for the TsvFile classes.
*/

// Read TsvFile-README.dox for an overview.

/**
\page file-format-tsv File Format: TSV (NON-OFFICIAL-RELEASE)

<center><b>DRAFT</b></center>

TSV files are simple tab separated line oriented text files. 

\section specs Specs & Features

- A TSV file may contain headers, one per line, in the format "#%key=value"
  - no leading whitespace is allowed on the line
  - key's may be listed more than once with different values
- A TSV file may contain comment lines in the format "##..."
  - no leading whitespace is allowed
- A TSV file must contain a line with column names (column header line)
- Column labels are case sensitive
- All the headers must appear before the column header line, tab separated
- The TSV file must contain zero or more lines of data, tab separated
- A TSV file may have DOS, *NIX, or MAC line endings
- WE NEED TO SAY SOMETHING ABOUT HOW ESCAPING OF CHARACTERS IS HANDLED
- WE NEED TO SAY SOMETHING ABOUT HOW QUOTING IS HANDLED
- WE NEED TO SAY SOMETHING ABOUT HOW BLANK LINES ARE HANDLED
- WE NEED TO SAY SOMETHING ABOUT WHAT CHARACTERS ARE ALLOWED WHERE

A varient of TSV files, v2, allowing for nested relationships is also supported. 
The presence of special headers indicates a v2 TSV file. For v2 TSV files
we have the addition features/specs:

- At a minimum, a header line for the top level must be provided which
  indicates the column names

\code
#%header0=col1-label	col2-label	col3-label
\endcode

- There is no separate column header line, the column header info
  is fully contained in the respective header

Multiple levels can be indicated with multiple header lines where the
trailing numerical digit indicates the level (top level is 0). So 
for example, a PGF file (which is a valid TSV file) has 3 levels:

\code
#%header0=probeset_id	type
#%header1=	atom_id
#%header2=		probe_id	type	gc_count	probe_length	interrogation_position	probe_sequence
2590411	main
	1
		5402769	pm:st	12	25	13	CGAAGTTGTTCATTTCCCCGAAGAC
\endcode

\section implementation Parsing and Writing

The official C++ parser used by affy can be found in APT under
sdk/file/TsvFile/TsvFile.{h,cpp}. When possible, parsing and 
writing of TSV files should be done using this code. 

It should be noted that this Parser, with the default settings,
will allow users to provide either a TSV or a CSV file.

See <a href="./TsvFile.html">TsvFile Design Notes</a> for
more implementation details.

\section notes Notes

The use of TSV files by the application usually imposes an expectation
that certain columns (as enumerated in the column header line) be 
present in the TSV file. For example, a cel list file for use in
apt-probeset-summarize with the --cel-files option assumes that the
TSV file has a "cel_files" column. No assumptions should be made about
the column order and there may be other columns present which the
software can ignore. 

\section example1 Example 1

As mentioned earlier, one example of a TSV file is the file
provided to apt-probeset-summarize using the --cel-files
option. This file must have a "cel_files" column.

\code
#%date=Tue Sep 26 11:54:09 PDT 2006
# These cel files are from the latin square study
pool	cel_files	scanner
1	sp1_a.cel	gcos1
1	sp1_b.cel	gcos2
1	sp1_c.cel	gcos3
2	sp2_a.cel	gcos1
2	sp2_b.cel	gcos2
2	sp2_c.cel	gcos3
\endcode

Now the same file, but with the tabs explicitly enumerated:

\code
#%date=Tue Sep 26 11:54:09 PDT 2006
# These cel files are from the latin square study
pool<tab>cel_files<tab>scanner
1<tab>sp1_a.cel<tab>gcos1
1<tab>sp1_b.cel<tab>gcos2
1<tab>sp1_c.cel<tab>gcos3
2<tab>sp2_a.cel<tab>gcos1
2<tab>sp2_b.cel<tab>gcos2
2<tab>sp2_c.cel<tab>gcos3
\endcode

\section related Related Pages

- <a href="./TsvFile.html">TsvFile Design Notes</a>
- <a href="./file-format-tsv.html">TSV File Format</a>

\section derived Derived Formats

- <a href="./file-format-clf.html">CLF File Format</a>
- <a href="./file-format-pgf.html">PGF File Format</a>

*/

/**
\page file-format-csv File Format: CSV (NON-OFFICIAL-RELEASE)

<center><b>DRAFT</b></center>

CSV files are simple comma separated line oriented text files. 

\section specs Specs & Features

- A CSV file may contain headers, one per line, in the format "#%key=value"
  - no leading whitespace is allowed on the line
  - key's may be listed more than once with different values
- A CSV file may contain comment lines in the format "##..."
  - no leading whitespace is allowed
- A CSV file must contain a line with column names (column header line)
- Column labels are case sensitive
- All the headers must appear before the column header line, comma separated
- The CSV file must contain zero or more lines of data, comma separated
- A CSV file may have DOS, *NIX, or MAC line endings
- WE NEED TO SAY SOMETHING ABOUT HOW ESCAPING OF CHARACTERS IS HANDLED
- WE NEED TO SAY SOMETHING ABOUT HOW QUOTING IS HANDLED
- WE NEED TO SAY SOMETHING ABOUT HOW BLANK LINES ARE HANDLED
- WE NEED TO SAY SOMETHING ABOUT WHAT CHARACTERS ARE ALLOWED WHERE

\section implementation Parsing and Writing

The official C++ parser used by affy can be found in APT under
sdk/file/TsvFile/TsvFile.{h,cpp}. When possible, parsing and 
writing of CSV files should be done using this code. 

It should be noted that this Parser, with the default settings,
will allow users to provide either a CSV or a TSV file.

See <a href="./TsvFile.html">TsvFile Design Notes</a> for
more implementation details.

\section notes Notes

The use of CSV files by the application usually imposes an expectation
that certain columns (as enumerated in the column header line) be 
present in the CSV file. For example, a cel list file for use in
apt-probeset-summarize with the --cel-files option assumes that the
CSV file has a "cel_files" column. No assumptions should be made about
the column order and there may be other columns present which the
software can ignore. 

\section example1 Example 1

The NetAffx annotation files are provided in CSV format. Below is
a toy example of a NetAffx annotation file in CSV format.

\code
#%create_date=Tue Jun 27 11:09:50 2006 PDT
#%lib-set-name=HuEx-1_0-st
#%lib-set-version=v2
#%genome-species=Homo sapiens
#%genome-version=hg17
#%netaffx-annotation-date=2006-Jan
#%netaffx-annotation-csv-version=1.7
#%netaffx-annotation-url-mrna=www.affymetrix.com/analysis/netaffx/exon/rna.affx?pk=
## probeset_id refers to the transcript cluster id, not the exon level probe set, and is required for ExACT software compatibility.
"transcript_cluster_id","probeset_id","seqname","strand","start","stop","total_probes","gene_assignment","mrna_assignment","swissprot","unigene","GO_biological_process","GO_cellular_component","GO_molecular_function","pathway","protein_domains","protein_families"
"7385707","7385707","chrX","-","144735330","144750034","8","---","---","---","---","---","---","---","---","---","---"
\endcode

\section related Related Pages

- <a href="./TsvFile.html">TsvFile Design Notes</a>
- <a href="./file-format-tsv.html">TSV File Format</a>

*/

#ifndef _TSV_FILE_H
#define _TSV_FILE_H

//
#include <fstream>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

// this comes with the affy SDK
#include "portability/affy-base-types.h"
#include "util/Err.h"

/// The default number of decimal places in output
#define TSV_DEFAULT_PRECISION 6
/// Access the rdbuf of the fstream directly.
#define TSV_USE_RDBUF 1

// ostia: fstream -O2 -ggdb
// real    1m43.776s
// user    0m37.475s
// sys     1m5.833s

// ostia: rdbuf -O2 -ggdb
// real    1m41.268s
// user    0m26.609s
// sys     1m7.651s

// ostia: rdbuf -O2
// real    1m36.234s
// user    0m26.962s
// sys     1m8.375s

namespace affx {
  class TsvFile;

// The "#ifndef SWIG" is used to avoid exporting the
// sections via swig. This keeps the interface small.

#ifndef SWIG
  class TsvFileBinding;
  class TsvFileField;
  class TsvFileHeaderLine;
  class TsvFileIndex;
#endif

  /// line numbers cant be negative
  typedef uint32_t linenum_t;

  const int TSV_INTERNSIZE_UNSET=-1;

  /// Selector for what is used to link the binding
  enum bindto_t {
    TSV_BINDTO_NONE  = 0, ///< The binding wont be bound
    TSV_BINDTO_CIDX  = 1, ///< Bound to column indexed by number
    TSV_BINDTO_CNAME = 2  ///< Bound to a column index by name
  };

  /// Return values to signal Error codes
  enum tsv_return_t {
    TSV_OK                   =  -1, ///< No Error
    TSV_LINE_BLANK           =  -2, ///< The line is blank          (m_lineLvl)
    TSV_LINE_COMMENT         =  -3, ///< The line is a comment line (m_lineLvl)
    TSV_ERR_UNKNOWN          = -11, ///< An otherwise unknown error
    TSV_ERR_NOTFOUND         = -12, ///< The value was not found
    TSV_ERR_NULL             = -13, ///< The value was null
    TSV_ERR_CONVERSION       = -14, ///< The value could not be converted
    TSV_ERR_FILEIO           = -15, ///< Some sort of file IO error
    TSV_ERR_FORMAT           = -16, ///< The format of the file is bad
    TSV_ERR_EOF              = -17, ///< Hit the end of the file
    TSV_HEADER               = -30, ///< A header line was read
    TSV_HEADER_LAST          = -31, ///< Did not read a header line
    TSV_LEVEL_LAST           = -32, ///< Did not read a line of the correct level
    TSV_FIND_LAST            = -33, ///< Found the last matching value
    TSV_LASTVALUE            = -99  ///< end of errors
  };
  
  /// Flags supplied to when finding matches
  enum tsv_op_t {
    TSV_OP_LT     = 0x01,
    TSV_OP_LTEQ   = 0x03,
    TSV_OP_EQ     = 0x02,
    TSV_OP_GTEQ   = 0x06,
    TSV_OP_GT     = 0x04
  };

  /// The datatype of a index
  enum tsv_indexkind_t {
    TSV_INDEX_STRING = 0x1,
    TSV_INDEX_INT    = 0x2,
    TSV_INDEX_DOUBLE = 0x3,
    TSV_INDEX_UINT   = 0x4,
    TSV_INDEX_ULONGLONG   = 0x5
  };
  /// Should the results be ordered by the line nubmer or value
  enum tsv_orderby_t {
    TSV_ORDERBY_LINE = 0x01,
    TSV_ORDERBY_VAL  = 0x02
  };
  /// The format of the file.
  enum tsv_fileformat_t {
    TSV_FORMAT_NONE  = 0x00,
    TSV_FORMAT_V1    = 0x01,
    TSV_FORMAT_V2    = 0x02,
    TSV_FORMAT_TABLE = 0x03,
    TSV_FORMAT_BAD   = 0xff
  };
  /// Requirements for binding. 
  enum tsv_bindopt_t {
    TSV_BIND_OPTIONAL = 0x01, /// bind if present
    TSV_BIND_WARNING  = 0x02, /// warn if not bound
    TSV_BIND_REQUIRED = 0x04, /// error if not bound
    TSV_BIND_NOFLAG   = 0x00  /// nothing
  };
  /// Flags for querying headers
  enum tsv_addflag_t {
    TSV_ADD_NONE     = 0x00, /// just for nothing...
    TSV_ADD_KEYS     = 0x01, /// return keys
    TSV_ADD_COMMENTS = 0x02, /// return comments
    TSV_ADD_ALL      = (TSV_ADD_KEYS|TSV_ADD_COMMENTS) /// keys and comments
  };

  // string functions
  void ltrim(std::string& str);
  void rtrim(std::string& str);
  void trim(std::string& str);
  void dequote(std::string& str);
  int  splitstr(std::string str,char c,std::vector<std::string>& vec);
  int unescapechar(int c);
#ifndef SWIG
  // dont expose this to swig
  bool header_ptr_less(const affx::TsvFileHeaderLine* a,const affx::TsvFileHeaderLine* b);
#endif
};

//////////

#ifndef SWIG

/// TsvFileBinding records the binding of a Column to a field.
/// When "tsv->bind()" is called, a TsvFileBinding is created to record
/// The details of the binding.  When a file is opened, the bindings are
/// linked to the correct Columns.
class affx::TsvFileBinding {
public:
  int            m_bindto_clvl;   ///< The column level
  affx::bindto_t m_bindto_type;   ///< Is is bound by index or name?
  int            m_bindto_cidx;   ///< The index of the bound column
  std::string    m_bindto_cname;  ///< The name of the bound column
  tsv_bindopt_t  m_flags;         ///< What options are flagged?
  
  int            m_interncache_size; ///< How many strings should be interned (reuse the storage)
  bool islinked;                  ///< Is this linked to a column?

  /// Only one of these should be set.
  /// The value of the column will be stored whereever they point.
  std::string* m_ptr_string;      ///< pointer to std::string
  int*         m_ptr_int;         ///< pointer to int
  double*      m_ptr_double;      ///< pointer to double
  float*       m_ptr_float;       ///< pointer to float
  unsigned int*  m_ptr_uint;      ///< pointer to unsigned int
  uint64_t*    m_ptr_ulonglong;   ///< pointer to unsigned long long

  ///
  TsvFileBinding();
  void clear();
};


//////////

/// TsvFileField holds the data read from a column.
/// The data is read into a std::string buffer.
/// Rather than resizing the buffer all the time, we try and
/// fill it "in place". Most of the time the new value should be the same
/// size as the old value.
/// We store the clvl and cidx info in the Column as well.
/// Saves having to search for it in the TsvFile object.
///
/// "cidx" is counted from *after* the leading tabs.
///

class affx::TsvFileField {
public:
  int         m_clvl;             ///< The indent level of the column
  int         m_cidx;             ///< The column number of the column.
  std::string m_cname;            ///< Column name. (Could be blank)
  bool m_optAutoDequote;          ///< Automatically strip quotes?
  bool m_optAutoTrim;             ///< Automatically strip white space?
  std::string m_buffer;           ///< Buffer space

  // copies of string for reuse
  int m_interncache_size;
  std::vector<std::string> m_interncache_vec;

  int m_precision;                ///< the output precision

  //
  bool m_isnull;                 ///< True if null
  bool m_ignore;           ///< If this column should be ignored

  //// Cached "int" conversions.  Once converted we dont need to do it again
  int    m_value_int;             ///< The cached value
  bool   m_value_int_done;        ///< Has this value been converted?
  int    m_value_int_rv;          ///< The return value for the conversion

  /// Cached "double" conversions.
  double m_value_double;          ///< same as int
  bool   m_value_double_done;     ///< same as int
  int    m_value_double_rv;       ///< same as int

  /// Cached "unsigned int" conversions.
  unsigned int m_value_uint;      ///< same as int
  bool   m_value_uint_done;       ///< same as int
  int    m_value_uint_rv;         ///< same as int

  /// Cached "uint64_t" conversions.
  uint64_t m_value_ulonglong;     ///< same as int
  bool   m_value_ulonglong_done;  ///< same as int
  int    m_value_ulonglong_rv;    ///< same as int

  std::vector<affx::TsvFileBinding*> m_linkedvar_vec; ///< Linked bindings

  //
  TsvFileField();
  ~TsvFileField();

  //
  bool isNull();
  void clear();
  //
  int setBuffer(std::string str);
  int setPrecision(int places);
  int setInternCacheSize(int size);

  // get the value of the column
  int get(std::string& val);
  int get(int&         val);
  int get(double&      val);
  int get(float&       val);
  int get(unsigned int& val);
  int get(uint64_t   & val);
  //
  int set(std::string val); ///< set the val
  int set(int         val); ///< set the val
  int set(double      val); ///< set the val
  int set(unsigned int val); ///< set the val
  int set(uint64_t    val); ///< set the val
#ifndef SWIG
  int set(float       val); ///< set the val
#endif
  ///
  void linkedvar_push(affx::TsvFileBinding* var);
  void linkedvars_clear();
  void linkedvars_assign();
};

//////////

///@brief TsvFileIndex maps values to line positions
class affx::TsvFileIndex {
public:
  /// Maybe we should inherit
  int            m_bindto_clvl;   ///< The column level
  affx::bindto_t m_bindto_type;   ///< Is is bound by index or name?
  int            m_bindto_cidx;   ///< The index of the bound column
  std::string    m_bindto_cname;  ///< The name of the bound column

  int  m_kind;   ///< The datatype being indexed (string,int,double)
  int  m_flags;  ///< flags for the index (unused)
  bool m_done;   ///< has the index been populated?

  ///@todo Change this to something denser
  std::multimap<std::string,linenum_t> m_index_str2line;    ///< map ofstring value to lines
  std::multimap<int        ,linenum_t> m_index_int2line;    ///< map of int values to lines
  std::multimap<double     ,linenum_t> m_index_double2line; ///< map of double values to lines
  std::multimap<unsigned int,linenum_t> m_index_uint2line;     ///< map of unsigned int values to lines
  std::multimap<uint64_t   ,linenum_t> m_index_ulonglong2line; ///< map of uint64_t values to lines

  //
  TsvFileIndex();
  ~TsvFileIndex();

  //
  void init();
  void clear();
  void data_add(TsvFileField* field,linenum_t line);
  //
  int results_append(std::vector<linenum_t>& results,int op,std::string val);
  int results_append(std::vector<linenum_t>& results,int op,int         val);
  int results_append(std::vector<linenum_t>& results,int op,double      val);
  int results_append(std::vector<linenum_t>& results,int op,unsigned int val);
  int results_append(std::vector<linenum_t>& results,int op,uint64_t    val);
  //
  void dump();

  // Templates are private to prevent accidental use.
private:
  template <typename T1, typename linenum_t> int results_append_tmpl(std::vector<linenum_t>& results,std::multimap<T1,linenum_t>& map,int op,T1& val);
  template<typename T1, typename linenum_t> void dump_map(std::multimap<T1,linenum_t>& map);
};


//////////

/// This is a Key value pair with sorting information.
class affx::TsvFileHeaderLine {
public:
  std::string m_key;
  std::string m_value;
  int m_order;     ///< order in which this header should be generated.
  /// normally equal to the line number)

  TsvFileHeaderLine();
  TsvFileHeaderLine(std::string key,std::string value,int order);

  //
  bool operator<(const affx::TsvFileHeaderLine& b) const;
};

//////////

#endif

/// @brief The tab seperated value file class
/// See the Docs in @link TsvFile TsvFile-README.dox @endlink
/// and @link tsv-example.cpp tsv-example.cpp @endlink .
class affx::TsvFile {
  //
  typedef TsvFileHeaderLine header_t;
  typedef std::multimap<std::string,header_t*>::iterator header_iter_t;

public:
  /// Options which can be set.
  bool m_optAllowDataComment;   ///< allow comments mixed in with data?
  bool m_optAbortOnError;       ///< ?
  bool m_optAutoColumns;        ///< add more columns as needed.
  bool m_optAutoDequote;        ///< remove quotes when reading?
  bool m_optAutoIndex;          ///< ?
  bool m_optAutoSenseSep;       ///< Autosense between tabs and commas
  bool m_optAutoTrim;           ///< remove whitespace from value?
  //char m_optCommentChar;        ///< Comment char
  bool m_optDoQuote;            ///< Put quotes on output?
  char m_optEscapeChar;         ///< the escape char to use
  bool m_optEscapeOk;           ///< Obey the escapechar?
  bool m_optHasColumnHeader;    ///< read the first line as column headers?
  bool m_optThrowOnError;       ///< ?
  unsigned char m_optQuoteChar; ///< Quote char to use
  std::string m_optEndl;        ///< End of line sequence to use
  unsigned char m_optFieldSep;  ///< Field seperator defaults to TAB.
  bool m_optHdrDblQuoteComma;   ///< Header contains '","'.
  bool m_optLinkVarsOnOpen;     ///< Link variables when calling open()

  int m_errno;         ///< The error number
  int  m_fileFormat;   ///< What version of file was read?

  /// The filename being visited
  std::string  m_fileName;

  /// Where the data begins (line 0)
  std::fstream::pos_type m_fileDataPos;

  /// Tag name of column header
  std::string m_headName;

private:

  // marked private to force use of the accessors.
  int m_lineNum; ///< The current line number
  int m_lineLvl; ///< The level of the current line

  // A handle for IO operations
  std::fstream m_fileStream;

  //public:
  std::fstream::pos_type m_line_fpos; ///< Where the current line starts

  //private:
  ///
  std::vector<header_t*> m_headers_vec;
  // where we are in the vector
  int m_headers_idx;
  header_t* m_headers_curptr;
  /// the key=val headers in string form.
  std::multimap<std::string,header_t*>  m_headers_bykey;
  /// used for headersNext
  header_iter_t m_headers_iter;
  std::string m_headers_curkey_string;

  /// The columns by column level and column index
  std::vector<std::vector<affx::TsvFileField> > m_column_map;
  /// Map the columns names to column indexes
  std::vector<std::map<std::string,int> > cnametocidx_map;

  /// The state of the bound and linked vars...
  bool m_linkvars_done;     ///< Are the links up to date
  int  m_linkvars_errors;   ///< Count of link errors
  int  m_linkvars_warnings; ///< Count of link warnings
  std::vector<TsvFileBinding*> m_linkvars_vec; ///< Vector of bindings linked to us

  /// Have the indexing been done?
  bool m_index_done;
  /// A map of line numbers to file postions
  std::vector<unsigned int> m_index_linefpos;
  /// A map of line numbers by level so we can find the parent lines
  std::vector<std::vector<unsigned int> > m_index_lineclvl;

  /// A vector of indexes which are defined.
  std::vector<TsvFileIndex*> m_index_vec;
  /// A vector of lines which match the last "findBegin"
  std::vector<unsigned int> m_findresults;
  /// The postition we are at in the returned results
  std::vector<unsigned int>::size_type m_findresults_pos;

public:
  //
  TsvFile();
  TsvFile(std::string fname);
  ~TsvFile();
  // Dont use the implicit copy or assigment...
  TsvFile(const TsvFile& that);
  TsvFile& operator=(const TsvFile& that) {
	Err::errAbort("TsvFile: Assigment of TsvFile not allowed.");
	return *this;
};
  //
  int setError(int err);
  int getError();
  int clearError();

  // these are linking errors but called binding errors...
  int bindErrorCnt();

  //
  linenum_t line_number();
  int line_level();

  int setFilename(std::string filename);
  /// @brief Get the filename of the TsvFile
  std::string getFileName() { return m_fileName; }
  int getLevelCount();
  int getColumnCount(int clvl);

  /// @brief Opens a file -- attempts to guess some defaults
  int open(std::string filename);
  /// Open a Csv file
  int openCsv(std::string filename);
  /// Opens a file as a table
  int openTable(std::string filename);
  /// Close the file
  int close();
  /// Closes the file and clears bindings and other info
  void clear();
  void clearBindings();
  void clearIndexes();
  void clearFields();
  void clearFieldsBelowClvl(int clvl_start);
  void clearHeaders();
  int  headerCount();
  /// Resets the options to the defaults
  void default_options();
  /// Check if file is opened
  int isFileOpen() { return (m_fileStream.is_open()? TSV_OK : TSV_ERR_FILEIO); }

  //
  int f_getline(std::string& line);
  bool f_lookingat_eol();
  int f_read_header_v1();
  int f_read_header_v2();
  int f_read_headers();
  int f_advance_tabs();
  int f_advance_eol();
  int f_read_column(affx::TsvFileField* col);
  int f_read_columns(int clvl);

  //
  void headers_to_fields_v2();

  /// User methods
  int defineColumn(int clvl,int cidx,std::string cname);

  //
  int defineFile(std::string definition);
  int defineFileParse(std::string definition);
  //
  int writeOpen(std::string filename);
  int writeCsv(std::string filename);
  int writeTsv(std::string filename);
  int writeTsv_v1(std::string filename);
  //
  int write_str(std::string& str);
  //
  void writeHeaders();
  void writeKeyValHeaders();
  void writeColumnHeaders();
  void writeColumnHeaders_clvl(int clvl);
  void writeFieldSep(int cnt);
  int writeLevel(int clvl);

  /// @todo should check for duplicate indexes and not make them
  int defineIndex(int clvl,std::string cname,int kind,int flags);
  int defineIndex(int clvl,int cidx         ,int kind,int flags);

  //
  int  getHeader(std::string key,std::string& val);
  int  getHeader(std::string key,int& val);

  /// @todo maybe add a flag to "addHeader" to skip checking?
  int  addHeader(std::string key,std::string val,int order);
  int  addHeader(std::string key,std::string val);
  int  addHeader(std::string key,int val);
  int  addHeadersFrom(affx::TsvFile& f_tsv,int flags);
  int  addHeadersFrom(affx::TsvFile& f_tsv,std::string prefix,int flags);
  int  addHeadersFrom(affx::TsvFile& f_tsv,std::string prefix,std::vector<std::string>& key_vec);
  int  addHeader_nocheck(std::string key,std::string val,int order);
  int  addHeader_nocheck(std::string key,std::string val);
  int  headerKeyLegal(std::string key);
  int  headersCount();
  void headersBegin();
  int  headersNext(std::string& key,std::string& val);
  int  headersFindNext(std::string key,std::string& val);
#ifndef SWIG
  TsvFileHeaderLine* nextHeaderPtr();
  int deleteHeaderPtr(TsvFileHeaderLine* hdrptr);
#endif
  int addHeaderComment(std::string comment);
  int addHeaderComment(std::string comment,int order);
  void resortHeaders();

  //
  int  cname2cidx(int clvl,int cidx);
  int  cname2cidx(int clvl,std::string cname);
#ifndef SWIG
  TsvFileField* clvlcidx2colptr(int clvl,int cidx);
  TsvFileField* clvlcidx2colptr(int clvl,std::string cname);
#endif
  ///  @todo rename this nicer...
  int  cidx2cname(int clvl,int cidx,std::string& cname);

  int setPrecision(int clvl,std::string cname,int places);
  int setPrecision(int clvl,int cidx,int places);

  //
#ifndef SWIG
  TsvFileBinding* linkvar_alloc();
  void linkvar_column(int clvl,int cidx,affx::TsvFileBinding* var);
  void linkvar_link(affx::TsvFileBinding* var);
#endif
  int  linkvars_makelinks();
  int  linkvars_maybe();
  void linkvars_clear();
  int formatOk();

#ifndef SWIG
  //
  void unbindAll();
  int bind(int clvl,std::string name ,std::string*  ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx         ,std::string*  ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,std::string name ,int*          ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx         ,int*          ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,std::string name ,double*       ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx         ,double*       ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,std::string name ,float*        ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx         ,float*        ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,std::string name ,unsigned int* ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx         ,unsigned int* ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,std::string name ,uint64_t*     ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx         ,uint64_t*     ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
#endif
  //
  void printBindingErrors();

  //
  void setInternCacheSize(int clvl,int cidx,int size);
  void setInternCacheSize(int clvl,std::string cname,int size);

  // movement commands.
  unsigned int lineNumber();
  int lineLevel();
  int nextLine();
  int nextLevel(int clvl);
  int rewind();
  int seekLine(unsigned int line);
  int gotoLine(unsigned int line);

  //
  bool isNull(int clvl,int cidx);
  bool isNull(int clvl,std::string cname);

  // Get a value given a clvl and cidx
  int get(int clvl,int cidx,std::string& val);
  int get(int clvl,int cidx,int&         val);
  int get(int clvl,int cidx,double&      val);
  int get(int clvl,int cidx,float&       val);
  int get(int clvl,int cidx,unsigned int& val);
  int get(int clvl,int cidx,uint64_t&    val);
  //
  int get(int clvl,std::string cname,std::string& val);
  int get(int clvl,std::string cname,int&         val);
  int get(int clvl,std::string cname,double&      val);
  int get(int clvl,std::string cname,float&       val);
  int get(int clvl,std::string cname,unsigned int& val);
  int get(int clvl,std::string cname,uint64_t&    val);
  //
  int set(int clvl,int cidx,std::string  val);
  int set(int clvl,int cidx,int          val);
  int set(int clvl,int cidx,double       val);
  int set(int clvl,int cidx,unsigned int val);
  int set(int clvl,int cidx,uint64_t     val);
  //
  int set(int clvl,std::string cname,std::string val);
  int set(int clvl,std::string cname,int         val);
  int set(int clvl,std::string cname,double      val);
  int set(int clvl,std::string cname,unsigned int val);
  int set(int clvl,std::string cname,uint64_t    val);

  //
#ifndef SWIG
  TsvFileIndex* index_alloc();
#endif
  int indexBuild();
  int indexBuildMaybe();
#ifndef SWIG
  //
  TsvFileIndex* index_matching(int clvl,int cidx,int kind);
  // for use by the template
  TsvFileIndex* index_matching_1(int clvl,int cidx,std::string& val);
  TsvFileIndex* index_matching_1(int clvl,int cidx,int val);
  TsvFileIndex* index_matching_1(int clvl,int cidx,double val);
  TsvFileIndex* index_matching_1(int clvl,int cidx,unsigned int val);
  TsvFileIndex* index_matching_1(int clvl,int cidx,uint64_t val);
#endif

  /// The error messages which one sees are nasty when using templates.
  /// This is the base template, make it private
private:
  template<typename T1,typename T2> int findBegin_tmpl(int clvl,T1 cidx,int op,T2 val,int flags);
public:
  /// These are instances of the above template
  /// The user will get a better error message
  int findBegin(int clvl,std::string cname,int op,std::string val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,std::string val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,std::string cname,int op,int         val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,int         val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,std::string cname,int op,double      val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,double      val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,std::string cname,int op,unsigned int val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,unsigned int val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,std::string cname,int op,uint64_t     val,int flags=TSV_ORDERBY_LINE);
  /// @brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,uint64_t     val,int flags=TSV_ORDERBY_LINE);
  //
  int findNext();
  //
  unsigned int findResultsCount();
  int findResultsClear();

#ifdef TSV_USE_RDBUF
  // This appears to be faster than calling ".get()"
  std::filebuf *m_rdbuf;
#endif

  /// used for debugging
  void dump();
  void dump_headers();
  void dump_data();
  void dump_bindings();
  void dump_indexes();

  ///
  void copyFormat(affx::TsvFile& f_tsv);
  int copyLevel(affx::TsvFile& f_tsv,int clvl);

  /// Return a pointer to ourself for swig.
  affx::TsvFile* tsv_ptr() { return this; };
};

//////////

#endif // TsvFile

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
 * @file   TsvFile.h
 * @brief  Headers for the TsvFile classes.
 *         Read @link file-format-tsv the TsvFile format @endlink for an overview.
 */

#ifndef _TSVFILE_H_
#define _TSVFILE_H_

// this comes with the affy SDK
#include "portability/affy-base-types.h"
#include "util/Err.h"
//
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>


/// The default number of decimal places in output
#define TSV_DEFAULT_PRECISION 6
/// Access the rdbuf of the fstream directly.
#define TSV_USE_RDBUF 1

//////////

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
    TSV_BINDTO_CNAME = 2, ///< Bound to a column index by name
  };

  enum valstate_t {
    VALSTATE_NONE=0,
    VALSTATE_INT=1,
    VALSTATE_DOUBLE=2,
    VALSTATE_STRING=3,
  };

  enum tsv_type_t {
    TSV_TYPE_UNKNOWN = 0,
    TSV_TYPE_ERR     = 1,
    TSV_TYPE_STRING  = 2,
    TSV_TYPE_INT     = 3,
    TSV_TYPE_FLOAT   = 4,
    TSV_TYPE_DOUBLE  = 5,
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
    //
    TSV_HEADER               = -30, ///< A header line was read
    TSV_HEADER_LAST          = -31, ///< Did not read a header line
    TSV_LEVEL_LAST           = -32, ///< Did not read a line of the correct level
    TSV_FIND_LAST            = -33, ///< Found the last matching value
    //
    TSV_ERR_BINARY           = -40, ///< This is some sort of binary file.
    TSV_ERR_CALVIN           = -41, ///< This is a calvin file.
    TSV_ERR_HDF5             = -42, ///< This is a HDF5 file.
    //
    TSV_LASTVALUE            = -99, ///< end of errors
  };

  /// Flags supplied to when finding matches
  enum tsv_op_t {
    TSV_OP_LT     = 0x01,
    TSV_OP_LTEQ   = 0x03,
    TSV_OP_EQ     = 0x02,
    TSV_OP_GTEQ   = 0x06,
    TSV_OP_GT     = 0x04,
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
  /// misc optional flags
  enum tsv_optionflag_t {
    TSV_OPT_NONE           = 0x00,
    TSV_OPT_CASESENSTIVE   = 0x01,
    TSV_OPT_CASEINSENSTIVE = 0x02,
  };

  // string functions
  void ltrim(std::string& str);
  void rtrim(std::string& str);
  void trim(std::string& str);
  //
  std::string tolower(const std::string& str);
  //
  void dequote(std::string& str);
  int  splitstr(const std::string& str,char c,std::vector<std::string>& vec);
  int  countchars(const std::string& str,char c);
  //
  int unescapeChar(int c);
  int escapeChar(int c);
  std::string escapeString(const std::string& str,const char escapeChar);
  //
  bool header_ptr_less(const affx::TsvFileHeaderLine* a,const affx::TsvFileHeaderLine* b);
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

  //
  TsvFileBinding();
  ~TsvFileBinding();
  //
  void clear();
  std::string get_type_str();
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
  tsv_type_t  m_ctype;
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

  int m_val_state;

  //// Cached "int" conversions.  Once converted we dont need to do it again
  int    m_value_int;             ///< The cached value
  bool   m_value_int_done;        ///< Has this value been converted?
  int    m_value_int_rv;          ///< The return value for the conversion

  ////
  int m_max_size;

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
  void init();
  void clear();
  bool isNull();
  bool isEmpty();

  //
  int setBuffer(const std::string& str);
  int setPrecision(int places);
  //int setInternCacheSize(int size);
  void convertToString();

  std::string get_name();
  affx::tsv_type_t get_type();
  int get_max_size();
  affx::tsv_type_t set_type(affx::tsv_type_t ctype);

  // get the value of the column
  int get(std::string*  val);
  int get(short*        val);
  int get(int*          val);
  int get(double*       val);
  int get(float*        val);
  int get(unsigned int* val);
  int get(uint64_t*     val);
  //
#ifndef SWIG
  int get(std::vector<int>* val,char sep=',');
  int get(std::vector<float>* val,char sep=',');
  int get(std::vector<double>* val,char sep=',');
  int get(std::vector<std::string>* val,char sep=',');
#endif

  //
  int set(const std::string& val); ///< set the val
  int set(int           val); ///< set the val
  int set(short         val); ///< set the val
  int set(double        val); ///< set the val
  int set(unsigned int  val); ///< set the val
  int set(uint64_t      val); ///< set the val

#ifndef SWIG
  int set(float         val); ///< set the val
#endif
  //
#ifndef SWIG
  int set(const std::vector<std::string>& val,char sep=',');
  int set(const std::vector<int>& val,char sep=',');
  int set(const std::vector<float>& val,char sep=',');
  int set(const std::vector<double>& val,char sep=',');
#endif

  ///
  void linkedvars_assign(affx::TsvFile* tsvfile);
  void linkedvar_push(affx::TsvFileBinding* var);
  void linkedvars_clear();
  //
  std::string get_bound_types_str();
};

//////////

/// @brief TsvFileIndex maps values to line positions
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

  // @todo Change this to something denser
  std::multimap<std::string,linenum_t> m_index_str2line;    ///< map of string value to lines
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

/// This is a key-value pair with sort order.
class affx::TsvFileHeaderLine {
public:
  std::string m_key;
  std::string m_value;
  int m_order;     ///< order in which this header should be put.
                   /// normally equal to the line number)

  TsvFileHeaderLine();
  TsvFileHeaderLine(const std::string& key,const std::string& value,int order);

  //
  bool operator<(const affx::TsvFileHeaderLine& b) const;

  void print();

};

//////////

#endif

/// \brief A class for reading and writing Tab Seperated Value (TSV) files.
///        See \link file-format-tsv the TsvFile format document \endlink
///        and the example program \link tsv-example.cpp tsv-example.cpp \endlink .
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
  bool m_optCheckFormatOnOpen;  ///< Automatically check for binary files?
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
  //
  char m_optQuoteChar1;         ///< Quoting Character
  char m_optQuoteChar2;         ///< Quoting Character
  //
  int m_optPrecision;           ///< Default Precision

  int m_errno;         ///< The error number
  bool m_eof;          ///< at eof?
  int  m_fileFormat;   ///< What version of file was read?

  /// The filename being visited
  std::string  m_fileName;

  /// Where the data begins (line 0)
  std::fstream::pos_type m_fileDataPos;

  /// Tag name of column header
  std::string m_headName;

private:

  bool m_rawOpen;
  
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
  bool m_headers_vec_packed;
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
  std::vector<std::map<std::string,int> > m_cnametocidx_map;

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
  TsvFile(const std::string& fname);
  ~TsvFile();

  // Dont use the implicit copy or assigment...
  TsvFile(const TsvFile& that);

#ifndef SWIG
  TsvFile& operator=(const TsvFile& that) {
#ifdef _MSC_VER
    (that); /* unused var */
#endif
    Err::errAbort("TsvFile: Assigment of TsvFile not allowed.");
    return *this;
  };
#endif

  //
  int setError(int err);
  int getError();
  int clearError();
  //
  void setAbortOnError(bool value);

  // these are linking errors but called binding errors...
  int bindErrorCnt();

  void flush();

  int setFilename(const std::string& filename);
  /// \brief Get the filename of the TsvFile
  std::string getFileName() { return m_fileName; }
  int getLevelCount();
  int getColumnCount(int clvl);
  static int getLineCountInFile(const std::string& filename, bool abortOnError = false);

  static int replaceCharInFile(const std::string &filename, char a, char b, bool abortOnError = true);
  
  /// \brief Opens a file -- attempts to guess some defaults
  int open(const std::string& filename);
  /// Open a Csv file
  int openCsv(const std::string& filename);
  /// Opens a file as a table
  int openTable(const std::string& filename);
  /// Close the file
  int close();
private:
  /// init a new object
  void init();
public:
  /// Closes the file and clears bindings and other info
  void clear();
  void clearBindings();
  void clearIndexes();
  void clearFields();
  void clearFieldsBelowClvl(int clvl_start);
  void clearColumnHeaders();
  void clearHeaders();
  int  headerCount();
  /// Resets the options to the defaults
  void default_options();
  /// Check if file is opened
  int isFileOpen() { return (m_fileStream.is_open()? TSV_OK : TSV_ERR_FILEIO); }
  // same as the filestream
  bool is_open() { return m_fileStream.is_open(); }
  bool good() { return m_fileStream.good(); }

  // These are internal methods.
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
  int defineColumn(int clvl,int cidx,const std::string& cname);
  int defineColumn(int clvl,int cidx,const std::string& cname,tsv_type_t ctype);
  /// @todo
  // int defineColumn(int clvl,int cidx,const std::vector<std::string>& cname,tsv_type_t ctype);

  //
  int defineFile(const std::string& definition);
  int defineFileParse(const std::string& definition);
  //
  tsv_return_t writeOpen(const std::string& filename);
  //
  tsv_return_t writeTsv(const std::string& filename);
  tsv_return_t writeTsv_v1(const std::string& filename);
  tsv_return_t writeTsv_v2(const std::string& filename);
  //
  tsv_return_t writeCsv(const std::string& filename);

  //
  int write_str(const std::string& str);
  //
  void writeHeaders();
  void writeKeyValHeaders();
  void writeColumnHeaders();
  void writeColumnHeaders_clvl(int clvl);
  void writeFieldSep(int cnt);
  int writeLevel(int clvl);

  /// @todo should check for duplicate indexes and not make them
  int defineIndex(int clvl,const std::string& cname,int kind,int flags);
  int defineIndex(int clvl,int cidx         ,int kind,int flags);

  //
  int getHeader(const std::string& key,std::string& val);
  int getHeader(const std::string& key,int& val);
  int getHeader(const std::string& key,double& val);
  int getHeader(const std::string& key,std::vector<std::string>& val);
  int getHeaderAppend(const std::string& key,std::vector<std::string>& val);
  // obtain the vector of header values whose header names contain the substring in key
  int getHeaderMatchingKeySubstr(const std::string& key,std::vector<std::string>& val);
  int getHeaderMatchingKeySubstrAppend(const std::string& key,std::vector<std::string>& val);

  // check to see if there is a header which is equal to val.
  // this is handy for checking chip types.
  int hasHeaderEqualTo(const std::string& key,const std::string& val);

  /// @todo maybe add a flag to "addHeader" to skip checking?
  int  addHeader(const std::string& key,const std::string& val,int order);
  int  addHeader(const std::string& key,const std::string& val);
  int  addHeader(const std::string& key,int val);
  int  addHeader(const std::string& key,double val);
  int  addHeader(const std::string& key,const std::vector<std::string>& val);

  //
  int  addHeadersFrom(affx::TsvFile& f_tsv,int flags);
  int  addHeadersFrom(affx::TsvFile& f_tsv,const std::string& prefix,int flags);
  int  addHeadersFrom(affx::TsvFile& f_tsv,const std::string& prefix,std::vector<std::string>& key_vec);
  int  addHeader_nocheck(const std::string& key,const std::string& val,int order);
  int  addHeader_nocheck(const std::string& key,const std::string& val);
  int  headerKeyLegal(const std::string& key);
  int  headersCount();
  void headersBegin();
  int  headersNext(std::string& key,std::string& val);
  int  headersFindNext(const std::string& key,std::string& val);
  //
  int  deleteHeaders(const std::string& key);
  int  deleteHeaders(const std::vector<std::string>& keys);
  int  deleteHeaders(const char** keys);
  //
  int printDuplicateHeaders();

#ifndef SWIG
  TsvFileHeaderLine* nextHeaderPtr();
  int deleteHeaderPtr(TsvFileHeaderLine* hdrptr);
#endif
  int addHeaderComment(const std::string& comment);
  int addHeaderComment(const std::string& comment,int order);
  int addHeaderComments(const std::vector<std::string>& comments);

  //
  void repackHeaders();
  void resortHeaders();
  //
  int writeFileComment(const std::string& comment);

  //
  int  cname2cidx(int clvl,int cidx);
  int  cname2cidx(int clvl,
                  const std::string& cname,
                  tsv_optionflag_t options=TSV_OPT_NONE);
  // find the first column name which matches.
  int  cname2cidx(int clvl,
                  const std::string& alias1,
                  const std::string& alias2,
                  tsv_optionflag_t options=TSV_OPT_NONE);
  int  cname2cidx(int clvl,
                  const std::string& alias1,
                  const std::string& alias2,
                  const std::string& alias3,
                  tsv_optionflag_t options=TSV_OPT_NONE);
  int  cname2cidx(int clvl,
                  const std::string& alias1,
                  const std::string& alias2,
                  const std::string& alias3,
                  const std::string& alias4,
                  tsv_optionflag_t options=TSV_OPT_NONE);

#ifndef SWIG
  TsvFileField* clvlcidx2colptr(int clvl,int cidx);
  TsvFileField* clvlcidx2colptr(int clvl,const std::string& cname);
#endif
  ///  @todo rename this nicer...
  int  cidx2cname(int clvl,int cidx,std::string& cname);
  std::string getColumnName(int clvl,int cidx);

  // set the default precision
  int setPrecision(int places);
  // set the precision on a column
  int setPrecision(int clvl,const std::string& cname,int places);
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
  void linkvars_free();
  //
  int formatOk();

#ifndef SWIG
  //
  void unbindAll();
  int bind(int clvl,const std::string& name ,std::string*  ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx                ,std::string*  ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,const std::string& name ,int*          ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx                ,int*          ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,const std::string& name ,double*       ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx                ,double*       ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,const std::string& name ,float*        ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx                ,float*        ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,const std::string& name ,unsigned int* ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx                ,unsigned int* ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,const std::string& name ,uint64_t*     ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
  int bind(int clvl,int cidx                ,uint64_t*     ptr,tsv_bindopt_t flags=TSV_BIND_OPTIONAL,int interncache_size=TSV_INTERNSIZE_UNSET);
#endif
  //
  void printBindingErrors();

  //
  //void setInternCacheSize(int clvl,int cidx,int size);
  //void setInternCacheSize(int clvl,std::string cname,int size);

  // info about the current line/row.
  unsigned int lineNumber();
  linenum_t lineNum();
  int lineLevel();
  bool eof();
  // movement commands.
  int nextLine();
  int nextLevel(int clvl);
  int rewind();
  int seekLine(linenum_t line);
  int gotoLine(linenum_t line);

  // Counts the total number of data lines in the file.
  // NOTE: rewinds the file back to the beginning.
  int countTotalDataLines();

  //
  void currentLineAsString(std::string& line);

  //
  bool isNull(int clvl,int cidx);
  bool isNull(int clvl,const std::string& cname);
  //
  bool isEmpty(int clvl,int cidx);
  bool isEmpty(int clvl,const std::string& cname);

  //
  affx::tsv_type_t get_type(int clvl,const std::string& cidx);
  affx::tsv_type_t get_type(int clvl,int cidx);
  affx::tsv_type_t set_type(int clvl,const std::string& cidx,affx::tsv_type_t type);
  affx::tsv_type_t set_type(int clvl,int cidx,affx::tsv_type_t type);

  //
  int clear_max_sizes();
  int deduce_types();
  int deduce_sizes();

  // Get a value given a clvl and cidx
  int get(int clvl,int cidx,std::string& val);
  int get(int clvl,int cidx,short&       val);
  int get(int clvl,int cidx,int&         val);
  int get(int clvl,int cidx,double&      val);
  int get(int clvl,int cidx,float&       val);
  int get(int clvl,int cidx,unsigned int& val);
  int get(int clvl,int cidx,uint64_t&    val);
  //
  int get(int clvl,const std::string& cname,std::string& val);
  int get(int clvl,const std::string& cname,short&       val);
  int get(int clvl,const std::string& cname,int&         val);
  int get(int clvl,const std::string& cname,double&      val);
  int get(int clvl,const std::string& cname,float&       val);
  int get(int clvl,const std::string& cname,unsigned int& val);
  int get(int clvl,const std::string& cname,uint64_t&    val);

#ifndef SWIG
  //
  int get(int clvl,int cidx,std::vector<int>* val,char sep=',');
  int get(int clvl,int cidx,std::vector<float>* val,char sep=',');
  int get(int clvl,int cidx,std::vector<double>* val,char sep=',');
  int get(int clvl,int cidx,std::vector<std::string>* val,char sep=',');
  //
  int get(int clvl,const std::string& cname,std::vector<int>* val,char sep=',');
  int get(int clvl,const std::string& cname,std::vector<float>* val,char sep=',');
  int get(int clvl,const std::string& cname,std::vector<double>* val,char sep=',');
  int get(int clvl,const std::string& cname,std::vector<std::string>* val,char sep=',');
#endif

  //
  int set(int clvl,int cidx,const std::string& val);
  //#ifndef SWIG
  int set(int clvl,int cidx,int          val);
  int set(int clvl,int cidx,float        val);
  int set(int clvl,int cidx,double       val);
#ifndef SWIG
  int set(int clvl,int cidx,unsigned int val);
#endif
  int set(int clvl,int cidx,uint64_t     val);
  //#endif
  //
  int set(int clvl,const std::string& cname,const std::string& val);
  int set(int clvl,const std::string& cname,int         val);
  int set(int clvl,const std::string& cname,float       val);
  int set(int clvl,const std::string& cname,double      val);
#ifndef SWIG
  int set(int clvl,const std::string& cname,unsigned int val);
#endif
  int set(int clvl,const std::string& cname,uint64_t    val);

#ifndef SWIG
  //
  int set(int clvl,int cidx,const std::vector<int>& val,char sep=',');
  int set(int clvl,int cidx,const std::vector<float>& val,char sep=',');
  int set(int clvl,int cidx,const std::vector<double>& val,char sep=',');
  int set(int clvl,int cidx,const std::vector<std::string>& val,char sep=',');
  //
  int set(int clvl,const std::string& cname,const std::vector<int>& val,char sep=',');
  int set(int clvl,const std::string& cname,const std::vector<float>& val,char sep=',');
  int set(int clvl,const std::string& cname,const std::vector<double>& val,char sep=',');
  int set(int clvl,const std::string& cname,const std::vector<std::string>& val,char sep=',');
#endif


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

  std::fstream::pos_type line_fpos();

  /// The error messages which one sees are nasty when using templates.
  /// This is the base template, make it private

private:
  template<typename T1,typename T2> int findBegin_tmpl(int clvl,T1 cidx,int op,T2 val,int flags);
public:
  /// These are instances of the above template
  /// The user will get a better error message
  int findBegin(int clvl,std::string cname,int op,std::string val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,std::string val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,std::string cname,int op,int         val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,int         val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,std::string cname,int op,double      val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,double      val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
#ifndef SWIG
  int findBegin(int clvl,std::string cname,int op,unsigned int val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,unsigned int val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,std::string cname,int op,uint64_t     val,int flags=TSV_ORDERBY_LINE);
  /// \brief like findBegin with different types
  int findBegin(int clvl,int cidx         ,int op,uint64_t     val,int flags=TSV_ORDERBY_LINE);
#endif
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

  /// put the column of a file into a vector. 
  /// the column and file are required to exist or we die.
  static int extractColToVec(const std::string& fileName,
                             const std::string& colName,
                             std::vector<std::string>* vec);
  static int extractColToVec(const std::string& fileName,
                             const std::string& colName,
                             std::vector<std::string>* vec,
                             int optEscapeOk);

  //
  static affx::tsv_type_t stringToColType(const std::string& str);

};

//////////

#endif // _TSVFILE_H_

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

/// @file   TsvFile.cpp
/// @brief  The implementation of TsvFile

//
#include "file/TsvFile/TsvFile.h"
//
#include "util/Convert.h"
#include "util/Err.h"
#include "util/Fs.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <string>

//
using namespace std;
using namespace affx;

// macros for TSV_ERR_AT stringification.
#define TSV_STRINGIFY(_str) #_str
#define TSV_TOSTR(_sym)   TSV_STRINGIFY(_sym)

// used to report where the error occured.
// @todo should set the error code too.
#define TSV_ERR_ABORT(_msg) APT_ERR_ABORT(_msg);


#ifdef TSV_USE_RDBUF
#define M_GETC()    (m_rdbuf->sbumpc())
#define M_PEEK()    (m_rdbuf->sgetc())
#define M_UNGETC(c) (m_rdbuf->sputbackc(c))
#else
#define M_GETC()    (m_fileStream.get())
#define M_PEEK()    (m_fileStream.peek())
#define M_UNGETC(c) (m_fileStream.putback(c))
#endif

// #define TSV_SET_ERR(err) (setError(err)))
#define TSV_SET_ERR(err)    (m_errno=err)

// Chars literials in numeric form (so they arent converted
// by the compiler) should be an enum?

/// Hex code for TAB char
#define TSV_CHAR_TAB     (0x09)
/// Hex code for LF char
#define TSV_CHAR_LF      (0x0a)
/// Hex code for a COMMA char
#define TSV_CHAR_COMMA   (0x2c)
/// Hex code for CR char
#define TSV_CHAR_CR      (0x0d)
/// Hex code for comment char
#define TSV_CHAR_COMMENT (0x23)
/// Hex code for SPACE char
#define TSV_CHAR_SPACE   (0x20)
/// code for end-of-file signal
#define TSV_CHAR_EOF     (-1)
/// Comment string
#define TSV_STRING_COMMENT  "#"
/// Meta string
#define TSV_STRING_META     "#%"

/// Unix End-of-line
#define TSV_EOL_UNIX   "\n"
/// Windows End-of-line
#define TSV_EOL_WIN    "\r\n"

// be nice to less fortunate users...
// use the native line endings
#ifdef WIN32
#define TSV_EOL TSV_EOL_WIN
#else
#define TSV_EOL TSV_EOL_UNIX
#endif

////////////////////

/// @brief     Trim whitespace from the left of a string
/// @param     str       the string to trim
void
affx::ltrim(std::string& str)
{
  string::size_type pos=str.find_first_not_of(" \t");
  if (pos==std::string::npos) {
    str.erase();
  } else {
    str.erase(0,pos);
  }
}

/// @brief     Trim whitespace from the right of a string
/// @param     str       the string to trim
void
affx::rtrim(std::string& str)
{
  string::size_type pos=str.find_last_not_of(" \t");
  if (pos==std::string::npos) {
    str.erase();
  } else {
    str.erase(pos+1);
  }
}

/// @brief     Trim whitespace from both sides of a string
/// @param     str       the string to trim
void
affx::trim(std::string& str)
{
  ltrim(str);
  rtrim(str);
}

// Turn a string into a lowercase string.
std::string affx::tolower(const string& str)
{
  std::string str_lc=str;
  for (size_t i=0;i<str_lc.size();i++) {
    // call the C function of the same name
    str_lc[i]=::tolower(str_lc[i]);
  }
  return str_lc;
}

/// @brief     Remove quotes from a string if they are matching
/// @param     str       String to remove them from
void
affx::dequote(std::string& str)
{
  // we only trim paired quotes
  size_t str_size=str.size();
  if (str_size<2) {
    return;
  }
  // pick out first and last char
  unsigned char c_begin=str[0];
  unsigned char c_end=str[str_size-1];
  // paired quotes?
  if (((c_begin=='"')||(c_begin=='\''))&&(c_begin==c_end)) {
    str.erase(str.size()-1,1); // remove last quote
    str.erase(0,1);            // remove first quote
  }
}

int
affx::countchars(const std::string& str,char chr)
{
  int cnt=0;
  for (size_t i=0;i<str.size();i++) {
    if (str[i]==chr) {
      cnt++;
    }
  }
  return cnt;
}

/// @brief     Split a string into a vector
/// @param     str       string to split
/// @param     c         character to split on
/// @param     vec       vector to fill with split strings
/// @return    number of resulting strings
int
affx::splitstr(const std::string& str,char c,std::vector<std::string>& vec)
{
  string::size_type pos1=0;
  string::size_type pos2=0;
  int num=0;

  vec.clear();

  while (1) {
    pos2=str.find(c,pos1);
    if (pos2==string::npos) {
      vec.resize(num+1);
      vec[num].assign(str,pos1,str.size()-pos1);
      break;
    } else {
      vec.resize(num+1);
      vec[num].assign(str,pos1,pos2-pos1);
      pos1=pos2+1;
    }
    ++num;
  }
  return num;
}

/// @brief     Convert the escaped char to the char which it represents
/// @param     c         the escaped char
/// @return    the converted char
int
affx::unescapeChar(int c)
{
  switch (c) {
  case 'b': return   8 ; // backspace
  case 'e': return  27 ; // escape
  case 'f': return  12 ; // form feed
  case 'n': return  TSV_CHAR_LF ;
  case 'r': return  TSV_CHAR_CR ;
  case 't': return  TSV_CHAR_TAB;
  case 'v': return  11; // vertical tab
    // return the character unchanged.
  default: return c;
  }
}

int
affx::escapeChar(int c)
{
  switch (c) {
  case  8:           return   'b' ; // backspace
  case 27:           return   'e' ; // escape
  case 12:           return   'f' ; // form feed
  case TSV_CHAR_LF:  return   'n' ;
  case TSV_CHAR_CR:  return   'r' ;
  case TSV_CHAR_TAB: return   't' ;
  case 11:           return   'v' ; // vertical tab
  case '#':          return   '#' ; // comment char
    // no escape needed.
  default: return 0;
  }
}
 
std::string
affx::escapeString(const std::string& str,const char eChar)
{
  int i_max=str.size();
  std::string estring;

  estring.reserve(i_max);

  for (int i=0;i<i_max;i++) {
    char c=str[i];
    char cc=escapeChar(c);
    if (cc!=0) {
      estring.append(1,eChar);
      estring.append(1,cc);
    }
    else if (c==eChar) {
      estring.append(1,eChar);
      estring.append(1,c);
    }
    else {
      estring.append(1,c);
    }
  }
  
  return estring;
}

////////////////////

/// @brief     Create a new Binding to map a column to a lvalue.
/// @return    a cleared out binding
affx::TsvFileBinding::TsvFileBinding()
{
  clear();
}

affx::TsvFileBinding::~TsvFileBinding()
{
  clear();
}

/// @brief     Clear all the members of the binding.
/// @return    void
void
affx::TsvFileBinding::clear()
{
  m_bindto_clvl=-1;
  m_bindto_type=TSV_BINDTO_NONE;
  m_bindto_cidx=-1;
  m_bindto_cname="";
  m_interncache_size=0;
  islinked=false;
  //
  m_ptr_string=NULL;
  m_ptr_int=NULL;
  m_ptr_double=NULL;
  m_ptr_float=NULL;
  m_ptr_uint=NULL;
  m_ptr_ulonglong=NULL;
}

std::string
TsvFileBinding::get_type_str()
{
  if (m_ptr_int!=NULL) {
    return "int";
  }
  if (m_ptr_string!=NULL) {
    return "string";
  }
  if (m_ptr_float!=NULL) {
    return "float";
  }
  if (m_ptr_double!=NULL) {
    return "double";
  }
  if (m_ptr_uint!=NULL) {
    return "uint";
  }
  if (m_ptr_ulonglong!=NULL) {
    return "longlong";
  }
  //
  return "unbound";
}

////////////////////

/// @brief     Creator of TsvFileField
/// @return    A cleared object
affx::TsvFileField::TsvFileField()
{
  init();
}

affx::TsvFileField::~TsvFileField()
{
  // nothing to deallocate
}

/// @brief     Init the internal bookkeeping data.  (Different than clear)
void
affx::TsvFileField::init()
{
  // internal indexes.
  m_clvl=-1;
  m_cidx=-1;
  m_cname="";
  // set default options
  m_ctype=affx::TSV_TYPE_UNKNOWN;
  m_optAutoDequote=true;
  //
  clear();
}

/// @brief     Clear the values of this feild.
void
affx::TsvFileField::clear()
{
  m_buffer="";
  m_isnull=true;
  //
  m_val_state=affx::VALSTATE_STRING;
  //
  m_value_int=0;
  m_value_int_done=false;
  m_value_int_rv=0;
  m_value_double=0.0;
  m_value_double_done=false;
  m_value_double_rv=0;
  m_value_uint=0;
  m_value_uint_done=false;
  m_value_uint_rv=0;
  m_value_ulonglong=0;
  m_value_ulonglong_done=false;
  m_value_ulonglong_rv=0;
  // default is 6 places "0.000000"
  m_precision=TSV_DEFAULT_PRECISION;
}

std::string
affx::TsvFileField::get_name() {
  return m_cname;
}

/// @brief     Return the expected type of the field.
/// @return
affx::tsv_type_t
affx::TsvFileField::get_type() {
  return m_ctype;
}

int
affx::TsvFileField::get_max_size() {
  return m_max_size;
}

/// @brief     Set the expected type of the field
/// @param     type
/// @return
affx::tsv_type_t
affx::TsvFileField::set_type(affx::tsv_type_t ctype) {
  m_ctype=ctype;
  return m_ctype;
}

/// @brief     Is the Column null?
/// @return    true if it is null
bool
affx::TsvFileField::isNull()
{
  return m_isnull;
}

bool
affx::TsvFileField::isEmpty()
{
  if (m_isnull||m_buffer=="") {
    return true;
  }
  return false;
}

/// @brief     Set the buffer value of the column
/// @param     str       The new value
int
affx::TsvFileField::setBuffer(const std::string& str)
{
  // dont alias the buffer. "m_buffer=str;" might leave m_buffer as a reference
  m_buffer.erase();
  m_buffer.append(str.begin(),str.end());
  //
  m_val_state=affx::VALSTATE_STRING;
  //
  m_isnull=false;
  m_value_int_done=false;
  m_value_double_done=false;
  m_value_uint_done=false;
  m_value_ulonglong_done=false;
  return TSV_OK;
}

int
affx::TsvFileField::setPrecision(int places)
{
  m_precision=places;
  return TSV_OK;
}

void
affx::TsvFileField::convertToString()
{
  switch (m_val_state) {
  case affx::VALSTATE_NONE:
    m_buffer.clear();
  case affx::VALSTATE_STRING:
    // do nothing; already a string
    break;
  case affx::VALSTATE_INT:
    {
      std::ostringstream stream;
      stream << m_value_int;
      setBuffer(stream.str());
    }
    break;
  case affx::VALSTATE_DOUBLE:
    {
      std::ostringstream stream;
      stream.setf(ios::fixed, ios::floatfield);
      stream.precision(m_precision);
      stream << m_value_double;
      setBuffer(stream.str());
    }
    break;
  default:
    TSV_ERR_ABORT("TsvFileField::convertToString(): internal error. m_val_state="+ToStr(m_val_state));
    break;
  }
}

/// handy macro for code.
#define TSV_NULL_RETURN(RV)  if (m_isnull) { *val=RV; return TSV_ERR_NULL; }

/// @brief     Get the value as a string
/// @param     val       val to set
/// @return    tsv_error_t
int
affx::TsvFileField::get(std::string* val)
{
  TSV_NULL_RETURN("");
  //
  if (m_val_state!=affx::VALSTATE_STRING) {
    // we need the string version, convert it...
    convertToString();
  }
  // Avoid data sharing by clearing then appending the contents.
  val->erase();
  val->append(m_buffer.begin(),m_buffer.end());
  return TSV_OK;
}

/// @brief     Get the value as an int
/// @param     val       val to set
/// @return    tsv_error_t
int
affx::TsvFileField::get(short* val)
{
  int tmp_val;
  int rv;

  // reset the val.
  *val=-1;

  //
  rv=get(&tmp_val);
  if (rv!=TSV_OK) {
    return rv;
  }
    
  if ((tmp_val<-32767)||(tmp_val>32767)) {
    return TSV_ERR_CONVERSION;
  }
  *val=tmp_val;
  return TSV_OK;
}

/// @brief     Get the value as an int
/// @param     val       val to set
/// @return    tsv_error_t
int
affx::TsvFileField::get(int* val)
{
  TSV_NULL_RETURN(0);
  // done already?
  if (m_value_int_done) {
    *val=m_value_int;
    return m_value_int_rv;
  }
  //
  const char* ptr_start=m_buffer.c_str();
  char* ptr_end=NULL;

  /// @todo: "" should be an error?

  // do the conversion (Force base 10)
  m_value_int=strtol(ptr_start,&ptr_end,10);
  // If m_ptr_end is not NULL then there was an invalid character.
  // If m_ptr_end == m_ptr_start then it was the null string. ("")
  if ((*ptr_end!=0)||(ptr_end==ptr_start)) {
    m_value_int=-1; // fill in with something off the end
    m_value_int_rv=TSV_ERR_CONVERSION; // signal an error
  }
  else {
    m_value_int_rv=TSV_OK;
  }

  m_value_int_done=true;
  *val=m_value_int;
  return m_value_int_rv;
}

/// @brief     Get the value as a double
/// @param     val       val to set
/// @return    tsv_error_t
int
affx::TsvFileField::get(double* val)
{
  TSV_NULL_RETURN(0.0);
  // done already?
  if (m_value_double_done) {
    *val=m_value_double;
    return m_value_double_rv;
  }
  //
  const char* ptr_start=m_buffer.c_str();
  char* ptr_end=NULL;

  // do the conversion
  m_value_double=strtod(ptr_start,&ptr_end);
  // same checks as strtol
  if ((*ptr_end!=0)||(ptr_end==ptr_start)) {
    m_value_double=-1.0; // bad value
    m_value_double_rv=TSV_ERR_CONVERSION; // signal an error
  } else {
    m_value_double_rv=TSV_OK;
  }

  m_value_double_done=true;
  *val=m_value_double;
  return m_value_double_rv;
}

/// @brief     Get the value as a float
/// @param     val       val to set
/// @return    tsv_error_t
int
affx::TsvFileField::get(float* val)
{
  if (m_value_double_done) {
    *val=(float)m_value_double;
    return m_value_double_rv;
  }
  // Get the double value and cast it.
  double tmp_double;
  int rv;
  rv=get(&tmp_double);
  *val=(float)tmp_double;
  return rv;
}

/// @brief     Get the value as an unsigned int
/// @param     val       val to set
/// @return    tsv_error_t
int
affx::TsvFileField::get(unsigned int* val)
{
  TSV_NULL_RETURN(0);
  // done already?
  if (m_value_uint_done) {
    *val=m_value_uint;
    return m_value_uint_rv;
  }
  //
  const char* m_ptr_start=m_buffer.c_str();
  char* m_ptr_end=NULL;

  // do the conversion
  m_value_uint=strtoul(m_ptr_start,&m_ptr_end,10);
  if ((*m_ptr_end)!=0) {
    m_value_uint=0; // fill in zero on error.
    m_value_uint_rv=TSV_ERR_CONVERSION; // signal it.
  } else {
    m_value_uint_rv=TSV_OK;
  }

  m_value_uint_done=true;
  *val=m_value_uint;
  return m_value_uint_rv;
}

/// @brief     Get the value as an uint64_t
/// @param     val       val to set
/// @return    tsv_error_t
int
affx::TsvFileField::get(uint64_t* val)
{
  TSV_NULL_RETURN(0);
  // done already?
  if (m_value_ulonglong_done) {
    *val=m_value_ulonglong;
    return m_value_ulonglong_rv;
  }
  //
  const char* m_ptr_start=m_buffer.c_str();
  char* m_ptr_end=NULL;

  // do the conversion
#ifndef WIN32
  m_value_ulonglong=strtoull(m_ptr_start,&m_ptr_end,10);
#else
  m_value_ulonglong=_strtoui64(m_ptr_start,&m_ptr_end,10);
#endif
  if ((*m_ptr_end)!=0) {
    m_value_ulonglong=0; // fill in zero on error.
    m_value_ulonglong_rv=TSV_ERR_CONVERSION; // signal it.
  } else {
    m_value_ulonglong_rv=TSV_OK;
  }

  m_value_ulonglong_done=true;
  *val=m_value_ulonglong;
  return m_value_ulonglong_rv;
}

/////

#define TSV_SPLIT_VEC_BODY(TYPE)                  \
  int rv=affx::TSV_OK;                            \
  string::size_type pos1;                         \
  string::size_type pos2;                         \
  char* pos_out;                                  \
  TYPE val;                                       \
                                                  \
  vec->clear();                                   \
                                                  \
  if (m_buffer=="") {                             \
    return rv;                                    \
  }                                               \
                                                  \
  pos1=0;                                         \
  while (1) {                                     \
    pos2=m_buffer.find(sep,pos1);                 \
    if (pos2==string::npos) {                     \
      val=CONVERT_FUNC();                         \
      vec->push_back(val);                        \
      break;                                      \
    } else {                                      \
      val=CONVERT_FUNC();                         \
      vec->push_back(val);                        \
      pos1=pos2+1;                                \
    }                                             \
  }                                               \
  return rv;

int
affx::TsvFileField::get(std::vector<int>* vec,char sep)
{
#define CONVERT_FUNC() strtol(&m_buffer[pos1],&pos_out,10)
  TSV_SPLIT_VEC_BODY(int);
#undef CONVERT_FUNC
}
int
affx::TsvFileField::get(std::vector<float>* vec,char sep)
{
#define CONVERT_FUNC() (float)strtod(&m_buffer[pos1],&pos_out)
  TSV_SPLIT_VEC_BODY(float);
#undef CONVERT_FUNC
}
int
affx::TsvFileField::get(std::vector<double>* vec,char sep)
{
#define CONVERT_FUNC() strtod(&m_buffer[pos1],&pos_out)
  TSV_SPLIT_VEC_BODY(double);
#undef CONVERT_FUNC
}

int
affx::TsvFileField::get(std::vector<std::string>* vec,char sep)
{
  int rv=affx::TSV_OK;
  string::size_type pos1;
  string::size_type pos2;
  std::string val;

  vec->clear();

  if (m_buffer=="") {
    return rv;
  }

  pos1=0;
  while (1) {
    pos2=m_buffer.find(sep,pos1);
    if (pos2==string::npos) {
      val.assign(m_buffer,pos1,m_buffer.size()-pos1);
      vec->push_back(val);
      break;
    } else {
      val.assign(m_buffer,pos1,pos2-pos1);
      vec->push_back(val);
      pos1=pos2+1;
    }
  }

  return rv;
}

//////////

#define TSV_CONCAT_VEC_BODY() {                 \
  if (vec.size()>0) {                           \
    stream << vec[0];                           \
    for (size_t i=1;i<vec.size();i++) {         \
      stream << sep << vec[i];                  \
    }                                           \
  }                                             \
  setBuffer(stream.str());                      \
  return TSV_OK;                                \
  }

int
affx::TsvFileField::set(const std::vector<std::string>& vec,char sep)
{
  std::ostringstream stream;
  TSV_CONCAT_VEC_BODY();
}
int
affx::TsvFileField::set(const std::vector<int>& vec,char sep)
{
  std::ostringstream stream;
  TSV_CONCAT_VEC_BODY();
}
int
affx::TsvFileField::set(const std::vector<float>& vec,char sep)
{
  std::ostringstream stream;
  stream.setf(ios::fixed, ios::floatfield);
  stream.precision(m_precision);
  TSV_CONCAT_VEC_BODY();
}
int
affx::TsvFileField::set(const std::vector<double>& vec,char sep)
{
  std::ostringstream stream;
  stream.setf(ios::fixed, ios::floatfield);
  stream.precision(m_precision);
  TSV_CONCAT_VEC_BODY();
}

#undef TSV_CONCAT_VEC_BODY

//////////

/// @brief     set the value of the buffer
/// @param     val       the new string value
/// @return    tsv_return_t
int
affx::TsvFileField::set(const std::string& val)
{
  return setBuffer(val);
}
/// @brief     set the value of the buffer
/// @param     val       the new value
/// @return    tsv_return_t
/// @todo      dont use ostringstream, use something faster
int
affx::TsvFileField::set(int val)
{
  m_value_int=val;
  m_value_int_done=true;
  m_value_int_rv=TSV_OK;
  m_val_state=affx::VALSTATE_INT;
  return TSV_OK;
}

#define TSV_CONVERT_FLOAT_BODY() {            \
  std::ostringstream stream;                  \
  stream.setf(ios::fixed, ios::floatfield);   \
  stream.precision(m_precision);              \
  stream << val;                              \
  return setBuffer(stream.str());             \
  }

/// @brief     set the value of the buffer
/// @param     val       the new value
/// @return    tsv_return_t
/// @todo      dont use ostringstream, use something faster
int
affx::TsvFileField::set(float val)
{
  //TSV_CONVERT_FLOAT_BODY();
  m_value_double=val;
  m_value_double_done=true;
  m_value_double_rv=TSV_OK;
  m_val_state=affx::VALSTATE_DOUBLE;
  return affx::TSV_OK;
}
/// @brief     set the value of the buffer
/// @param     val       the new value
/// @return    tsv_return_t
/// @todo      dont use ostringstream, use something faster
int
affx::TsvFileField::set(double val)
{
  m_value_double=val;
  m_value_double_done=true;
  m_value_double_rv=affx::TSV_OK;
  m_val_state=affx::VALSTATE_DOUBLE;
  return affx::TSV_OK;
}

#undef TSV_CONVERT_FLOAT_BODY

/// @brief     set the value of the buffer
/// @param     val       the new value
/// @return    tsv_return_t
/// @todo      dont use ostringstream, use something faster
int
affx::TsvFileField::set(unsigned int val)
{
  // not used as often, just convert it to a string now.
  std::ostringstream stream;
  stream << val;
  return setBuffer(stream.str());
}
/// @brief     set the value of the buffer
/// @param     val       the new value
/// @return    tsv_return_t
/// @todo      dont use ostringstream, use something faster
int
affx::TsvFileField::set(uint64_t val)
{
  // not used as often, just convert it to a string now.
  std::ostringstream stream;
  stream << val;
  return setBuffer(stream.str());
}

//////////

/// @brief     Add the var to the collection of linked vars
/// @param     var       var to add
void
affx::TsvFileField::linkedvar_push(affx::TsvFileBinding* var)
{
  m_linkedvar_vec.push_back(var);
}
/// @brief     Clear the linkages to bound variables.
void
affx::TsvFileField::linkedvars_clear()
{
  m_linkedvar_vec.clear();
}
/// @brief     Assign each linked var a value.
void
affx::TsvFileField::linkedvars_assign(affx::TsvFile* tsvfile)
{
  // Assign to the linked vars
  size_t lvar_vec_size=m_linkedvar_vec.size();
  int rv=TSV_OK;

  for (size_t vi=0;vi<lvar_vec_size;vi++) {
    TsvFileBinding* var=m_linkedvar_vec[vi];
    if (var!=NULL) {
      // These are sorted by frequency of use.
      if (var->m_ptr_int!=NULL) {
        rv=get(var->m_ptr_int);
      }
      else if (var->m_ptr_string!=NULL) {
        rv=get(var->m_ptr_string);
      }
      else if (var->m_ptr_float!=NULL) {
        rv=get(var->m_ptr_float);
      }
      else if (var->m_ptr_double!=NULL) {
        rv=get(var->m_ptr_double);
      }
      else if (var->m_ptr_uint!=NULL) {
        rv=get(var->m_ptr_uint);
      }
      else if (var->m_ptr_ulonglong!=NULL) {
        rv=get(var->m_ptr_ulonglong);
      }
      else {
        TSV_ERR_ABORT("internal error: Binding does not have a pointer!");
      }
      //
      if ((rv!=TSV_OK)&&(var->m_flags&TSV_BIND_REQUIRED)) {
        // We say "data line" to make it clear we arent counting header lines.
        std::string errmsg="Conversion error\n";
        errmsg+="TSVERROR: '"+tsvfile->m_fileName+"':"+ToStr(tsvfile->lineNumber()+1)+": ";
        errmsg+="Conversion of required field '"+m_cname+"'"+
          " to "+var->get_type_str()+
          " of '"+m_buffer+"' failed!\n";
        TSV_ERR_ABORT(errmsg);
      }
    }
  }
}

std::string
affx::TsvFileField::get_bound_types_str()
{
  std::string rv="";
  size_t lvar_vec_size=m_linkedvar_vec.size();

  for (size_t vi=0;vi<lvar_vec_size;vi++) {
    TsvFileBinding* var=m_linkedvar_vec[vi];
    if (var!=NULL) {
      // These are sorted by frequency of use.
      if (var->m_ptr_int!=NULL) {
        rv+="int,";
      }
      else if (var->m_ptr_string!=NULL) {
        rv+="string,";
      }
      else if (var->m_ptr_float!=NULL) {
        rv+="float,";
      }
      else if (var->m_ptr_double!=NULL) {
        rv+="double,";
      }
      else if (var->m_ptr_uint!=NULL) {
        rv+="uint,";
      }
      else if (var->m_ptr_ulonglong!=NULL) {
        rv+="longlong,";
      }
    }
  }
  // trim trailing ","
  if (rv!="") {
    rv.erase(rv.size()-1);
  }
  return rv;
}

/* Unused -- code which wants this generally uses <std::map>
int
affx::TsvFileField::setInternCacheSize(int size)
{
  int old_size=m_interncache_size;
  // -1 means ignore this setting.
  if (size==TSV_INTERNSIZE_UNSET) {
    return old_size;
  }
  // Zero disables it, positive size enables it
  m_interncache_size=size;
  m_interncache_vec.reserve(size);
  if (size<(int)m_interncache_vec.size()) {
    // this is not random replacement but is quick.
    m_interncache_vec.resize(size);
  }
  //
  return old_size;
}
*/

////////////////////

/// @brief Create a TsvFileIndex
affx::TsvFileIndex::TsvFileIndex()
{
  init();
}
/// @brief Destroy a TsvFileIndex
affx::TsvFileIndex::~TsvFileIndex()
{
}

/// @brief     Init the member vars to known values
void
affx::TsvFileIndex::init()
{
  m_bindto_cidx=-1;
  m_bindto_clvl=-1;
  m_bindto_cname="";
  m_bindto_type=TSV_BINDTO_NONE;
  m_flags=0;
  m_kind=0;
  //
  clear();
}

/// @brief     Clear the contents of the index
void
affx::TsvFileIndex::clear()
{
  m_done=false;
  m_index_str2line.clear();
  m_index_int2line.clear();
  m_index_double2line.clear();
  m_index_uint2line.clear();
  m_index_ulonglong2line.clear();
}

/// @brief     Add the data from field to the index
/// @param     field     The field to index
/// @param     line      The line number on which it was found
void
affx::TsvFileIndex::data_add(TsvFileField* field,linenum_t line)
{
  APT_ERR_ASSERT(field!=NULL,"internal error: data_add: field is null.");
  //
  if (m_kind==TSV_INDEX_STRING) {
    // WARNING: field->m_buffer is reused and will be overwritten.
    // So we need to force a fresh copy to be made by modifying the string.
    // so first we "copy" the string.
    // std::string str_copy=field->m_buffer;
    // then we modify it -- this forces a copy
    // str_copy.append(1,' ');
    // then we undo the modification
    // str_copy.resize(str_copy.size()-1);
    // now we can use it in our map
    std::string str_copy;
    str_copy.append(field->m_buffer.begin(),field->m_buffer.end());
    m_index_str2line.insert(make_pair(str_copy,line));
    return;
  }
  if (m_kind==TSV_INDEX_INT) {
    int tmp_int;
    if (field->get(&tmp_int)==TSV_OK) {
      m_index_int2line.insert(make_pair(tmp_int,line));
    }
    return;
  }
  if (m_kind==TSV_INDEX_DOUBLE) {
    double tmp_double;
    if (field->get(&tmp_double)==TSV_OK) {
      m_index_double2line.insert(make_pair(tmp_double,line));
    }
    return;
  }
  if (m_kind==TSV_INDEX_UINT) {
    unsigned int tmp_uint;
    if (field->get(&tmp_uint)==TSV_OK) {
      m_index_uint2line.insert(make_pair(tmp_uint,line));
    }
    return;
  }
  if (m_kind==TSV_INDEX_ULONGLONG) {
    uint64_t tmp_ulonglong;
    if (field->get(&tmp_ulonglong)==TSV_OK) {
      m_index_ulonglong2line.insert(make_pair(tmp_ulonglong,line));
    }
    return;
  }
}

/// @brief     Template to query an index for matching values
/// @param     results   Where to stick the line numbers
/// @param     T1        Datatype of the maps
/// @param     map       map to search
/// @param     op        comparison operator
/// @param     val       value to compare against
/// @return    tsv_return_t
template<typename T1, typename linenum_t>
int
affx::TsvFileIndex::results_append_tmpl(std::vector<linenum_t>& results,
                                     std::multimap<T1,linenum_t>& mmap,
                                     int op,
                                     T1& val)
{
  typename std::multimap<T1,linenum_t>::iterator r_start,r_end;

  if (op==TSV_OP_LT) {
    r_start=mmap.begin();
    r_end=mmap.lower_bound(val);
  }
  else if (op==TSV_OP_LTEQ) {
    r_start=mmap.begin();
    r_end=mmap.upper_bound(val);
  }
  else if (op==TSV_OP_EQ) {
    r_start=mmap.lower_bound(val);
    r_end=mmap.upper_bound(val);
  }
  else if (op==TSV_OP_GTEQ) {
    r_start=mmap.lower_bound(val);
    r_end=mmap.end();
  }
  else if (op==TSV_OP_GT) {
    r_start=mmap.upper_bound(val);
    r_end=mmap.end();
  } else {
    TSV_ERR_ABORT("Invalid operation in results_append. op="+ToStr(op));
    return TSV_ERR_UNKNOWN;
  }

  // Stick the range into find_result
  while (r_start!=r_end) {
    //printf("r_start=(%4d,%4d)\n",(*r_start).first,(int)(*r_start).second);
    results.push_back((*r_start).second);
    r_start++;
  }
  //
  return TSV_OK;;
}

/// @brief     Append the matching string results to the vec
/// @param     results   where to append
/// @param     op        comparison to do
/// @param     val       value to compare to
/// @return    tsv_return_t
int
affx::TsvFileIndex::results_append(std::vector<linenum_t>& results,int op,std::string val)
{
  // Use the template for the different types
  return results_append_tmpl(results,m_index_str2line,op,val);
}
/// @brief     Append the matching string results to the vec
/// @param     results   where to append
/// @param     op        comparison to do
/// @param     val       value to compare to
/// @return    tsv_return_t
int
affx::TsvFileIndex::results_append(std::vector<linenum_t>& results,int op,int val)
{
  return results_append_tmpl(results,m_index_int2line,op,val);
}
/// @brief     Append the matching string results to the vec
/// @param     results   where to append
/// @param     op        comparison to do
/// @param     val       value to compare to
/// @return    tsv_return_t
int
affx::TsvFileIndex::results_append(std::vector<linenum_t>& results,int op,double val)
{
  return results_append_tmpl(results,m_index_double2line,op,val);
}
/// @brief     Append the matching string results to the vec
/// @param     results   where to append
/// @param     op        comparison to do
/// @param     val       value to compare to
/// @return    tsv_return_t
int
affx::TsvFileIndex::results_append(std::vector<linenum_t>& results,int op,unsigned int val)
{
  return results_append_tmpl(results,m_index_uint2line,op,val);
}
/// @brief     Append the matching string results to the vec
/// @param     results   where to append
/// @param     op        comparison to do
/// @param     val       value to compare to
/// @return    tsv_return_t
int
affx::TsvFileIndex::results_append(std::vector<linenum_t>& results,int op,uint64_t val)
{
  return results_append_tmpl(results,m_index_ulonglong2line,op,val);
}

/// @brief     Dump the contents of an index
/// @param     T1        Datatype of the maps key
/// @param     map       the map to dump
template<typename T1, typename linenum_t>
void
affx::TsvFileIndex::dump_map(std::multimap<T1,linenum_t>& mmap)
{
  typename std::multimap<T1,linenum_t>::iterator i;
  int cnt=0;
  for (i=mmap.begin();i!=mmap.end();i++) {
    cout << cnt++ << " : '" << (*i).first << "' : '" << (*i).second << "'\n";
  }
}

/// @brief     Dump the contents of the index
void
affx::TsvFileIndex::dump()
{
  printf("index (clvl=%2d,cidx=%2d,kind=%2d) ==========\n",m_bindto_clvl,m_bindto_cidx,m_kind);

  if (m_kind==TSV_INDEX_STRING) {
    dump_map(m_index_str2line);
  }
  if (m_kind==TSV_INDEX_INT) {
    dump_map(m_index_int2line);
  }
  if (m_kind==TSV_INDEX_DOUBLE) {
    dump_map(m_index_double2line);
  }
  if (m_kind==TSV_INDEX_UINT) {
    dump_map(m_index_uint2line);
  }
  if (m_kind==TSV_INDEX_ULONGLONG) {
    dump_map(m_index_ulonglong2line);
  }
}

////////////////////

/// @brief   Init the slots of the object.
affx::TsvFileHeaderLine::TsvFileHeaderLine()
{
  m_key="";
  m_value="";
  m_order=0;
}

/// @brief  Init the slots to the values given.
affx::TsvFileHeaderLine::TsvFileHeaderLine(const std::string& key,const std::string& value,int order)
{
  m_key=key;
  m_value=value;
  m_order=order;
}

/// @brief  Less than operator for comparing TsvFileHeaderLines
bool
affx::TsvFileHeaderLine::operator<(const affx::TsvFileHeaderLine& b) const
{
  if (m_order==b.m_order) {
    return (m_key<b.m_key);
  }
  return (m_order<b.m_order);
}

void
affx::TsvFileHeaderLine::print()
{
  if (m_key=="") {
    printf("#%s\n",m_value.c_str());
  }
  else {
    printf("#%%%s=%s\n",m_key.c_str(),m_value.c_str());
  }
}


/// @brief  Function for comparing TsvFileHeaderLine pointers
bool
affx::header_ptr_less(const affx::TsvFileHeaderLine* a,const affx::TsvFileHeaderLine* b)
{
  // sort nulls to the end
  if ((a==NULL)&&(b==NULL)) {
    return false;
  }
  if (b==NULL) {
    return true;
  }
  if (a==NULL) {
    return false;
  }

  return ((*a)<(*b));
}

////////////////////

/// @brief     Create and clear.
/// @return    lvalue
affx::TsvFile::TsvFile()
{
  init();
  clear();
  default_options();
}
/// @brief     Create, clear and open a tsvfile
/// @param     filename
/// @return    lvalue
affx::TsvFile::TsvFile(const std::string& filename)
{
  init();
  clear();
  open(filename);
}

/// @brief  Dont allow a TsvFile to be copied.
affx::TsvFile::TsvFile(const affx::TsvFile& that)
{
  init();
	TSV_ERR_ABORT("Copy of TsvFile not allowed.");
}

/// @brief     Destroy a TsvFile
/// @return    lvalue
affx::TsvFile::~TsvFile()
{
  clear(); // this will close the file handle.
  linkvars_free();
}

/// @brief     set options to their defaults
void
affx::TsvFile::default_options()
{
  m_rawOpen = false;
  m_optAllowDataComment=true;
  m_optAbortOnError=true;
  m_optAutoColumns=false;
  m_optAutoDequote=true;
  m_optAutoIndex=true;
  m_optAutoSenseSep=true;
  m_optAutoTrim=false;
  m_optCheckFormatOnOpen=true;
  //m_optCommentChar='#';
  m_optDoQuote=false;
  // This seems to be a common use case. - jhg
  // If you really care, then call tsv->setEscapeOk(whatEverYouWant)
  // #ifdef WIN32
  //   m_optEscapeOk=false;
  // #else
  m_optEscapeOk=true;
  // #endif
  m_optEscapeChar='\\';
  m_optQuoteChar1='\'';
  m_optQuoteChar2='"';
  m_optFieldSep=TSV_CHAR_TAB;
  m_optHasColumnHeader=true;
  m_optQuoteChar='"';
  m_optThrowOnError=false;
  m_optEndl=TSV_EOL;
  m_optLinkVarsOnOpen = true;
  m_headName = "header";
  m_optPrecision=-1;
}

/// @brief     Set the option abortonerror
/// @param     value     the new value.
void
affx::TsvFile::setAbortOnError(bool value)
{
  m_optAbortOnError=value;
}

/// @brief     Returns the value of m_eof.
/// @return    should be true when at the end of the file.
bool
affx::TsvFile::eof()
{
  return m_eof;
}

/// @brief     What line did we just read? (external)
/// @return    The line number of the line which was just read.
linenum_t
affx::TsvFile::lineNumber()
{
  // line_num is the NEXT LINE to be read, so subtract one
  return m_lineNum-1;
}

/// @brief     The indentation level of the line just read.
/// @return    the level
int
affx::TsvFile::lineLevel()
{
  return m_lineLvl;
}

/// @brief     Set the filename for this Tsv
/// @param     filename
/// @return
int
affx::TsvFile::setFilename(const std::string& filename)
{
  m_fileName=filename;
  return TSV_OK;
}

/// @brief init a new TsvFile.
void
affx::TsvFile::init()
{
#ifdef TSV_USER_RDBUF
  m_rdbuf=NULL;
#endif
  m_headers_curptr=NULL;
}

/// @brief     clear all the resources of a TsvFile.
void
affx::TsvFile::clear()
{
  // close the file before zapping everything...
  close();
  // ...zap the filename
  m_fileName.clear();

  // dispose of all the bindings.
  clearBindings();
  // and the indexes
  clearIndexes();
  // and the headers
  clearHeaders();
  // and the column headers
  clearColumnHeaders();
  
  //
  m_lineLvl=0;
  m_lineNum=0;
  m_eof=false;
  m_index_done=false;
}

/// @brief Clear the bindings of columns to variables.
void
affx::TsvFile::clearBindings()
{
  // clear the linked vars in the columns
  linkvars_clear();
  // release the memory of the vars
  for (unsigned int vi=0;vi<m_linkvars_vec.size();vi++) {
    delete m_linkvars_vec[vi];
  }
  // forget the pointers
  m_linkvars_vec.clear();
  // done because there arent any.
  m_linkvars_done=true;
}

/// @brief     Clear (ie delete) all the indexes
void
affx::TsvFile::clearIndexes()
{
  for (unsigned int i=0;i<m_index_vec.size();i++) {
    if (m_index_vec[i]!=NULL) {
      m_index_vec[i]->clear();
      delete m_index_vec[i];
    }
  }
}

/// @brief     Clear all the fields.  (set them all to null)
void
affx::TsvFile::clearFields()
{
  clearFieldsBelowClvl(0);
}

/// @brief     Clear the columns at greater indents
/// @param     clvl_start  Clear fields at or larger than this level.
void
affx::TsvFile::clearFieldsBelowClvl(int clvl_start)
{
  assert(clvl_start>=0);

  unsigned int clvl_size=m_column_map.size();
  for (unsigned int clvl=clvl_start;clvl<clvl_size;clvl++) {
    unsigned int cidx_size=m_column_map[clvl].size();
    for (unsigned int cidx=0;cidx<cidx_size;cidx++) {
      m_column_map[clvl][cidx].clear();
      // dont zero out the linked vars -- while they should be zeroed,
      // it takes time.
      // m_column_map[clvl][cidx].linkedvars_assign();
    }
  }
}

//////////

/// @brief     Set the error value
/// @param     err       The error to set
/// @return    tsv_error_t
int
affx::TsvFile::setError(int err)
{
  //
  m_errno=err;
  if (m_errno==TSV_OK) {
    return m_errno;
  }

  //
  if (m_optAbortOnError==true) {
    assert(0);
  }
  //
  return err;
}

/// @brief     clear the error
/// @return    tsv_error_t
int
affx::TsvFile::clearError()
{
  return setError(TSV_OK);
}

/// @brief     Return the current error number.
/// @return
int
affx::TsvFile::getError()
{
  return m_errno;
}

//////////

/// @brief     The line number of the current line (internal)
/// @return    number
linenum_t
affx::TsvFile::lineNum()
{
  return m_lineNum;
}

/// @brief     The number of levels defined
/// @return    clvl
int
affx::TsvFile::getLevelCount()
{
  return (int)m_column_map.size();
}
/// @brief     The number of columns for this level
/// @param     clvl      the level we are asking about
/// @return    cidx
int
affx::TsvFile::getColumnCount(int clvl)
{
  if ((clvl>=0)&&(clvl<(int)m_column_map.size())) {
    return (int)m_column_map[clvl].size();
  }
  return 0;
}

/// @brief     Are we looking at the end of a line?
/// @return    true if at end of line.
bool
affx::TsvFile::f_lookingat_eol()
{
  int c;
  // the end of a file is the end of a line.
  if (!m_fileStream.good()) {
    return true;
  }
  //
  //c=m_fileStream.peek();
  c=M_PEEK();
  if (c==EOF) {
    return true;
  }
  if ((c==TSV_CHAR_CR)||(c==TSV_CHAR_LF)) {
    return true;
  }
  return false;
}

/// @brief     Read a line from a TsvFile into a string.
/// @param     line    A buffer to modify
/// @return    tsv_return_t
/// @remarks   Lines end with (0x0a "\n") (0x0d "\r") or (0x0d 0x0a "\r\n")
int
affx::TsvFile::f_getline(std::string& line)
{
  int c;

  line.clear();
  // we have some sort of error
  if (!m_fileStream.good()) {
    return (TSV_ERR_FILEIO);
  }

  while (1) {
    // get a char...
    //c=m_fileStream.get();
    c=M_GETC();
    // eof?
    if ((c==TSV_CHAR_EOF)||!m_fileStream.good()) {
      break;
    }
    // LF ends a line
    if (c==TSV_CHAR_LF) {
      break;
    }
    // CR ends a line too.
    if (c==TSV_CHAR_CR) {
      // c=m_fileStream.peek();
      c=M_PEEK();
      // CR could be followed by a LF
      if (c==TSV_CHAR_LF) {
        //c=m_fileStream.get();
        c=M_GETC();
        break;
      }
    }
    // push it on
    line.append(1,(char)c);
  }
  //
  return TSV_OK;
}



//////////

/// @brief     clear all the headers.
void
affx::TsvFile::clearHeaders()
{
  m_headers_bykey.clear();
  //
  int v_size=(int)m_headers_vec.size();
  for (int vi=0;vi<v_size;vi++) {
    delete m_headers_vec[vi];
    m_headers_vec[vi]=NULL;
  }
  //
  m_headers_vec.clear();
  m_headers_vec_packed=true;
  // clear these too...
  headersBegin();
}

/// @brief     clear all the column headers.
void
affx::TsvFile::clearColumnHeaders()
{

  // now get rid of the column info.
  m_column_map.clear();
  m_cnametocidx_map.clear();

}

/// @brief     Start at the beginning of the headers
/// @remarks   This acts as the begin for all the ways of walking the headers.
void
affx::TsvFile::headersBegin()
{
  resortHeaders();
  m_headers_idx=0;
  m_headers_iter=m_headers_bykey.begin();
  m_headers_curptr=NULL;
  m_headers_curkey_string="";
}

/// @brief Return the next non-null header pointer.  NULL when at or past the end
TsvFileHeaderLine*
affx::TsvFile::nextHeaderPtr()
{
  while (m_headers_idx<(int)m_headers_vec.size()) {
    m_headers_curptr=m_headers_vec[m_headers_idx++];
    if (m_headers_curptr!=NULL) {
      return m_headers_curptr;
    }
  }
  m_headers_curptr=NULL;
  return m_headers_curptr;
}

/// @brief     Advance to the next header matching key
/// @param     key       key of the header
/// @param     val       value of the header matched
/// @return
int
affx::TsvFile::headersFindNext(const std::string& key,std::string& val)
{
  // if the key is different that the last, do an implicit begin
  if (key!=m_headers_curkey_string) {
    headersBegin();
    m_headers_curkey_string=key;
    m_headers_iter=m_headers_bykey.lower_bound(key);
  }

  // past the end of matching keys?
  if ((m_headers_iter==m_headers_bykey.end())||(m_headers_iter->first!=key)) {
    return TSV_HEADER_LAST;
  }

  // what we are returning...
  val=m_headers_iter->second->m_value;
  // point to the next one...
  m_headers_iter++;
  return TSV_OK;
}

/// @brief     Advance to the next header of the file and set the values
/// @param     key       key of the header
/// @param     val       value of the header
/// @return
int
affx::TsvFile::headersNext(std::string& key,std::string& val)
{
  // skip comment headers
  do {
    nextHeaderPtr();
    if (m_headers_curptr==NULL) {
      return TSV_HEADER_LAST;
    }
  } while (m_headers_curptr->m_key=="");

  key=m_headers_curptr->m_key;
  val=m_headers_curptr->m_value;

  // headers next should never return an empty key.
  assert(key!="");

  return TSV_OK;
}

/// @brief     How many headers are there?
/// @return    Count of the headers
int
affx::TsvFile::headersCount()
{
  // test here to save a method call.
  if (m_headers_vec_packed==false) {
    repackHeaders();
  }
  return (int)m_headers_vec.size();
}

/// @brief     Is the key a legal key to have in a header?
/// @param     key       the key to check
/// @return    tsv_return_t
int
affx::TsvFile::headerKeyLegal(const std::string& key)
{
  // We dont allow '=' in the key
  if (key.find('=')!=std::string::npos) {
    return (TSV_ERR_FORMAT);
  }
  // Nor do we allow headers which start with "header" -- they are reserved
  if ((key.size()==(m_headName.length()+1))&&(key.find(m_headName)==0)) {
    return (TSV_ERR_FORMAT);
  }
  // if not rejected it is ok...
  return TSV_OK;
}

/// @brief     Add a key-value to the headers
/// @param     key       string key
/// @param     val       string value
/// @remarks   sort order defaults to "0"
int
affx::TsvFile::addHeader(const std::string& key,const std::string& val)
{
  // should a copy be done here?
  // might that be the source of the windows problems?
  return addHeader(key,val,(int)m_headers_vec.size());
}

/// @brief     Add a key-value to the headers plus its sort order.
/// @param     key       string key
/// @param     val       string val
/// @param     order     the sort order
/// @return    tsv_error_t
int
affx::TsvFile::addHeader(const std::string& key,const std::string& val,int order)
{
  int rv;
  if ((rv=headerKeyLegal(key))!=TSV_OK) {
    return rv;
  }
  return addHeader_nocheck(key,val,order);
}

/// @brief     Add a int value as a header
/// @param     key       name of header
/// @param     val       value to set
/// @return
int
affx::TsvFile::addHeader(const std::string& key,int val)
{
  // cast to a string and add it.
  std::ostringstream stream;
  stream << val;
  addHeader(key,stream.str());
  return TSV_OK;
}

/// @brief     Add a int value as a header
/// @param     key       name of header
/// @param     val       double value to set
/// @return
int
affx::TsvFile::addHeader(const std::string& key,double val)
{
  // cast to a string and add it.
  std::ostringstream stream;
  stream << val;
  std::string valString = stream.str();
  // For Windows, check to see if a three digit exponent with a leading zero is
  // being used for double representation.  if so, remove the extra zero (2
  // exponent digits conforms to 1999 C standard) 
  //e.g. 1.234e003 and 1.234e-003 become 1.234e03 and 1.234e-03
  #ifdef WIN32
  if (valString.size() >= 5 &&
      valString.at(valString.size() - 3) == '0' && 
      (valString.at(valString.size() - 4) == 'e' ||
       valString.at(valString.size() - 5) == 'e'        
       )
      ) {
    valString.erase(valString.size() - 3, 1);
  }
  #endif
  addHeader(key,valString);
  return TSV_OK;
}

/// @brief     Add a series of headers as key=val1, key=val2, key=valN
/// @param     key       name of header
/// @param     val       vector of values to add.
/// @return    
int
affx::TsvFile::addHeader(const std::string& key,const std::vector<std::string>& val)
{
  for (size_t i=0;i<val.size();i++) {
    addHeader(key,val[i]);
  }
  return TSV_OK;
}

/// @brief     Add a comment to the headers
/// @param     comment   string comment
/// @param     order     sort order
/// @return    tsv_error_t
int
affx::TsvFile::addHeaderComment(const std::string& comment,int order)
{
  return addHeader_nocheck("",comment,order);
}

/// @brief     Add a comment to the headers
/// @param     comment   string comment
/// @return    tsv_error_t
/// @remarks   sort order defaults to order added
int
affx::TsvFile::addHeaderComment(const std::string& comment)
{
  return addHeader_nocheck("",comment,(int)m_headers_vec.size());
}

int
affx::TsvFile::addHeaderComments(const std::vector<std::string>& comments)
{
  for (size_t i=0;i<comments.size();i++) {
    addHeaderComment(comments[i]);
  }
  return TSV_OK;
}

/// @brief     Add a header to the TSV without checking to see if it is legal
/// @param     key       string key
/// @param     val       string val
/// @return    tsv_error_t
int
affx::TsvFile::addHeader_nocheck(const std::string& key,const std::string& val)
{
  return addHeader_nocheck(key,val,(int)m_headers_vec.size());
}

/// @brief     Writes a comment directly to the file.
/// @param     comment
/// @return    tsv_error_t
int
affx::TsvFile::writeFileComment(const std::string& comment)
{
  if (!m_fileStream.good()) {
    TSV_ERR_ABORT("File is not writeable.");
  }
  m_fileStream << TSV_STRING_COMMENT << comment << m_optEndl;
  //
  return TSV_OK;
}

/// @brief     Add a header to the TSV without checking to see if it is legal
/// @param     key       key
/// @param     val       value
/// @param     order     sort order
/// @return    TSV_OK
int
affx::TsvFile::addHeader_nocheck(const std::string& key,const std::string& val,int order)
{
  TsvFileHeaderLine* hdrptr=new TsvFileHeaderLine(key,val,order);
  //
  m_headers_vec.push_back(hdrptr);
  m_headers_bykey.insert(make_pair(key,hdrptr));
  return TSV_OK;
}

/// @brief     Add all the headers from another TSV to this one.
/// @param     f_tsv     where to copy the headers from
/// @param     flags     what kind of headers to add (TSV_ADD_COMMENTS,TSV_ADD_KEYS)
/// @return    TSV_OK
int
affx::TsvFile::addHeadersFrom(affx::TsvFile& f_tsv,int flags)
{
  return addHeadersFrom(f_tsv,"",flags);
}

/// @brief     append the headers from another TSV to this one.
/// @param     f_tsv     TSV to copy headers from
/// @param     prefix    string to prefix keys with
/// @param     flags     which kind of headers to add (TSV_ADD_COMMENTS,TSV_ADD_KEYS)
/// @return    tsv_error_t
int
affx::TsvFile::addHeadersFrom(affx::TsvFile& f_tsv,const std::string& prefix,int flags)
{
  string key;
  string val;

  f_tsv.headersBegin();
  while (f_tsv.headersNext(key,val)==TSV_OK) {
    if (key=="") {
      if ((flags&TSV_ADD_COMMENTS)!=0) {
        addHeaderComment(val);
      }
    } else {
      if ((flags&TSV_ADD_KEYS)!=0) {
        addHeader(prefix+key,val);
      }
    }
  }
  return TSV_OK;
}

/// @brief     Add headers specified in a vector to this TSV from another TSV
/// @param     f_tsv     source of TSV headers
/// @param     prefix    prefix to add to keys
/// @param     key_vec   vector of keys to add
/// @return    tsv_error_t
int
affx::TsvFile::addHeadersFrom(affx::TsvFile& f_tsv,
                              const std::string& prefix,
                              std::vector<std::string>& key_vec)
{
  string val;

  for (int i=0;i<(int)key_vec.size();i++) {
    if (f_tsv.getHeader(key_vec[i],val)==TSV_OK) {
      addHeader(prefix+key_vec[i],val);
    }
  }
  return TSV_OK;
}

/// @brief     Note that the "headerN" lines may not have been generated yet.
/// @return    The number of headers for this file.
int
affx::TsvFile::headerCount()
{
  repackHeaders(); // this removes null ptrs so headerCount==size()
  return m_headers_vec.size();
}

/// @brief     Get the value of a header
/// @param     key       which header to get
/// @param     val       string to bind the value to
/// @return    tsv_return_t
int
affx::TsvFile::getHeader(const std::string& key,std::string& val)
{
  m_headers_iter=m_headers_bykey.find(key);
  if (m_headers_iter==m_headers_bykey.end()) {
    return TSV_ERR_NOTFOUND;
  }
  val=(*m_headers_iter).second->m_value;
  return TSV_OK;
}

/// @brief     Get the integer value of a header
/// @param     key       key to get
/// @param     val       integer value to be modfied.
/// @return    tsv_error_t
int
affx::TsvFile::getHeader(const std::string& key,int& val)
{
  std::string tmp_str;
  const char* str_in;
  char* str_out;

  if (getHeader(key,tmp_str)==TSV_OK) {
    str_in=tmp_str.c_str();
    int tmp_val=strtol(str_in,&str_out,0);
    if (str_in==str_out) {
      return TSV_ERR_FORMAT;
    }
    val=tmp_val;
    return TSV_OK;
  }
  return TSV_ERR_NOTFOUND;
}

/// @brief     Get the integer value of a header
/// @param     key       key to get
/// @param     val       integer value to be modfied.
/// @return    tsv_error_t
int
affx::TsvFile::getHeader(const std::string& key,double& val)
{
  std::string tmp_str;
  const char* str_in;
  char* str_out;

  if (getHeader(key,tmp_str)==TSV_OK) {
    str_in=tmp_str.c_str();
    double tmp_val=strtod(str_in,&str_out);
    if (str_in==str_out) {
      return TSV_ERR_FORMAT;
    }
    val=tmp_val;
    return TSV_OK;
  }
  return TSV_ERR_NOTFOUND;
}

/// @brief     Get string values of all headers exactly matching a key
/// @param     key       key to find
/// @param     val       string vector for matching values.
/// @return    tsv_error_t
int
affx::TsvFile::getHeader(const std::string& key,std::vector<std::string>& val)
{
  val.clear();
  return getHeaderAppend(key,val);
}

/// @brief     Append string values of all headers exactly matching a key
/// @param     key       key to find
/// @param     val       string vector for appended values.
/// @return    tsv_error_t
int
affx::TsvFile::getHeaderAppend(const std::string& key,std::vector<std::string>& val)
{
  int result = TSV_ERR_NOTFOUND;
  affx::TsvFile::header_iter_t i;
  for (i=m_headers_bykey.find(key);(i!=m_headers_bykey.end())&&(i->first==key);i++) {
    val.push_back(i->second->m_value);
    result = affx::TSV_OK;
  }
  return result;
}

/// @brief     Get string values of all headers containing the substring
/// @param     key       key substring to find
/// @param     val       string vector for matching values.
/// @return    tsv_error_t
int
affx::TsvFile::getHeaderMatchingKeySubstr(const std::string& key,std::vector<std::string>& val)
{
  val.clear();
  return getHeaderMatchingKeySubstrAppend(key,val);
}

/// @brief     Append string values of all headers containing the substring
/// @param     key       key substring to find
/// @param     val       string vector for appended values.
/// @return    tsv_error_t
int
affx::TsvFile::getHeaderMatchingKeySubstrAppend(const std::string& key,std::vector<std::string>& val)
{
  int result = TSV_ERR_NOTFOUND;
  affx::TsvFile::header_iter_t i;
  for (i=m_headers_bykey.begin();i!=m_headers_bykey.end();i++) {
	if (i->first.find(key)!=string::npos)
	{
      val.push_back(i->second->m_value);
      result = affx::TSV_OK;
	}
  }
  return result;
}

/// @brief     Does the file have a key==value header which matches
/// @param     key       key to check
/// @param     val       value to check
/// @return    true if there is a matching key==val header
/// @remarks   This is handy to check for matching chip types.
int
affx::TsvFile::hasHeaderEqualTo(const std::string& key,const std::string& val)
{
  // printf("hasHeaderEqualTo('%s','%s')\n",key.c_str(),val.c_str());
  // scan the headers.
  affx::TsvFile::header_iter_t i;
  for (i=m_headers_bykey.find(key);(i!=m_headers_bykey.end())&&(i->first==key);i++) {
    // printf("hasHeaderEqualTo: check '%s'/'%s')\n",i->first.c_str(),i->second->m_value.c_str());
    if (i->second->m_value==val) {
      return TSV_OK;
    }
  }
  // didnt find it.
  return TSV_ERR_NOTFOUND;
}

void
affx::TsvFile::repackHeaders()
{
  if (m_headers_vec_packed==true) {
    return;
  }

  //
  std::vector<affx::TsvFileHeaderLine*> tmp_vec;
  std::vector<affx::TsvFileHeaderLine*>::iterator i;

  // save the non-null ones...
  for (i=m_headers_vec.begin();i!=m_headers_vec.end();i++) {
    if (*i!=NULL) {
      tmp_vec.push_back(*i);
    }
  }
  // ...and put them back.
  m_headers_vec=tmp_vec;
  // we are packed
  m_headers_vec_packed=true;
}


/// @brief     Sort the headers of this TSV file by (order,key,value)
void
affx::TsvFile::resortHeaders()
{
  //
  repackHeaders();
  // put headers in order
  sort(m_headers_vec.begin(),m_headers_vec.end(),affx::header_ptr_less);
}

/// @brief     Remove the headers named with the key.
/// @param     key the header key value.
/// @return    tsv_error_t

int
affx::TsvFile::deleteHeaders(const std::string& key)
{
  int v_size=(int)m_headers_vec.size();
  for (int vi=0;vi<v_size;vi++) {
    if ((m_headers_vec[vi]!=NULL) && (m_headers_vec[vi]->m_key==key)) {
      deleteHeaderPtr(m_headers_vec[vi]);
    }
  }
  return affx::TSV_OK;
}

int
affx::TsvFile::deleteHeaders(const std::vector<std::string>& keys)
{
  for (size_t i=0;i<keys.size();i++) {
    deleteHeaders(keys[i]);
  }
  return affx::TSV_OK;
}

int
affx::TsvFile::deleteHeaders(const char**  keys)
{
  if (keys!=NULL) {
    for (int i=0;keys[i]!=NULL;i++) {
      deleteHeaders(keys[i]);
    }
  }
  return affx::TSV_OK;
}

/// @brief     Remove the header pointed to
/// @param     hdrptr    pointer to the header to delete
/// @return    tsv_error_t
int
affx::TsvFile::deleteHeaderPtr(affx::TsvFileHeaderLine* hdrptr)
{
  // skip if done...
  if (hdrptr==NULL) {
    return TSV_OK;
  }

  // find the header ptrs in the map and erase them
  affx::TsvFile::header_iter_t mi;
  // Goto is an ugly hag to work around win32 stl bug
  START_DEL_HEADER_PTR:
  for (mi=m_headers_bykey.begin();mi!=m_headers_bykey.end();mi++) {
    if (mi->second==hdrptr) {
      m_headers_bykey.erase(mi);
      goto START_DEL_HEADER_PTR;
    }
  }
  // erase it from the vector, dont resize it.
  int find_cnt=0;
  int v_size=(int)m_headers_vec.size();
  for (int vi=0;vi<v_size;vi++) {
    if (m_headers_vec[vi]==hdrptr) {
      m_headers_vec[vi]=NULL;
      find_cnt++;
    }
  }
  // delete the pointer if it was found in the list.
  if (find_cnt>0) {
    m_headers_vec_packed=false;
    delete hdrptr;
    return TSV_OK;
  }

  // didnt find it in the list.
  return TSV_ERR_NOTFOUND;
}

//////////

/// @brief     Read a classic "v1" header from the top of a file
/// @return    tsv_return_t
/// @remarks   Attempts to guess if this is a tab or comma seperated file
int
affx::TsvFile::f_read_header_v1()
{
  int rv;
  std::string line;
  std::vector<std::string> colname_vec;

  if ((rv=f_getline(line))!=TSV_OK) {
    return rv;
  }

  // Try and sense what the fieldsep is?
  if (m_optAutoSenseSep==true) {
    int cnt_fieldsep=countchars(line,m_optFieldSep);
    if (cnt_fieldsep==0) {
      // whoah We didnt see our fieldsep, take a guess...
      int cnt_tabs=countchars(line,TSV_CHAR_TAB);
      int cnt_comma=countchars(line,TSV_CHAR_COMMA);
      if (cnt_tabs>0) {
        m_optFieldSep=TSV_CHAR_TAB;
      }
      else if (cnt_comma>0) {
        m_optFieldSep=TSV_CHAR_COMMA;
      }
    }
  }

  // set the columns
  splitstr(line,m_optFieldSep,colname_vec);
  for (unsigned int col=0;col<colname_vec.size();col++) {
    // discard quotes?
    if (m_optAutoDequote==true) {
      dequote(colname_vec[col]);
    }
    //
    defineColumn(0,col,colname_vec[col]);
  }
  return TSV_OK;
}

/// @brief     Read and parse a v2 header line
int
affx::TsvFile::f_read_header_v2()
{
  int rv;
  fstream::pos_type f_start;
  string line;

  f_start=m_fileStream.tellg();

  if ((rv=f_getline(line))!=TSV_OK) {
    return rv;
  }

  // printf("header: %s\n",line.c_str());

  ltrim(line);
  // "#%" An actual header line
  if (line.find("#%")==0) {
    string::size_type pos_equal=line.find("=");
    if (pos_equal!=string::npos) {
      string key(line,2,pos_equal-2);
      string val(line,pos_equal+1);
      trim(key);
      rtrim(val);
      // printf("key=%-20s val=%s\n",key.c_str(),val.c_str());
      addHeader_nocheck(key,val);
      return TSV_HEADER;
    }
    // should print a warning.
    return (TSV_ERR_FORMAT);
  }
  // skip the comment
  if (line.find("#")==0) {
    addHeaderComment(string(line,1));
    return TSV_HEADER;
  }

  // not a header line - skip back to start of line to read as data
  m_fileStream.seekg(f_start);
  return TSV_HEADER_LAST;
}

/// @brief     Read all the headers and set up the file
/// @return    tsv_return_t
int
affx::TsvFile::f_read_headers()
{
  // skip to start of file.
  m_fileStream.seekg(0);

  // suck in all the v2 headers
  while (f_read_header_v2()==TSV_HEADER) {
    // null body
  }

  //dump_headers();

  // If we dont have any headers at all, then the first line has the header
  // read its header line.
  std::string ignored;
  if (getHeader(m_headName+"0",ignored)!=TSV_OK) {
    if (m_optHasColumnHeader==true) {
      m_fileFormat=TSV_FORMAT_V1;
      f_read_header_v1();
    } else {
      m_fileFormat=TSV_FORMAT_TABLE;
    }
  } else {
    m_fileFormat=TSV_FORMAT_V2;
    headers_to_fields_v2();
  }

  // The rest of the file is data.
  m_fileDataPos=m_fileStream.tellg();
  // sets line counters
  rewind();

  //dump_bindings();
  return TSV_OK;
}

/// @brief     Parse the "headerN" lines
void
affx::TsvFile::headers_to_fields_v2()
{
  // now set up mappings
  for (unsigned int clvl=0;clvl<9;clvl++) {
    string head_name=m_headName;
    head_name.append(1,(char)('0'+clvl)); // +"num"
    affx::TsvFile::header_iter_t i=m_headers_bykey.find(head_name);
    if (i!=m_headers_bykey.end()) {
      /// @todo discard the leading spaces and split on whitespace?
      vector<string> colname_vec;
      splitstr(i->second->m_value,m_optFieldSep,colname_vec);
      for (unsigned int col=0;col<colname_vec.size();col++) {
        if (col<clvl) {
          if (colname_vec[col]!="") {
            Verbose::out(1, "==Warning: level=" + ToStr(clvl) + " col=" + ToStr(col) +
                         " name=" + colname_vec[col] +" not indended correctly==");
//             printf("==Warning: level=%d col=%d name='%s' not indended correctly\n",
//                    clvl,col,colname_vec[col].c_str());
          }
        } else {
          defineColumn(clvl,col-clvl,colname_vec[col]);
        }
      }
    }
  }
}

//////////


/// @brief     Define a column with a
/// @param     clvl
/// @param     cidx
/// @param     cname
/// @param     ctype
/// @return
int affx::TsvFile::defineColumn(int clvl,int cidx,const std::string& cname)
{
  return defineColumn(clvl,cidx,cname,TSV_TYPE_UNKNOWN);
}

/// @brief     record the mapping of column names (cname) to column indexes (cidx).
/// @param     clvl      column level the mapping is for
/// @param     cidx      position the column is at
/// @param     cname     name of the column
/// @remarks   resizes the maps as needed


int
affx::TsvFile::defineColumn(int clvl,int cidx,const std::string& cname,tsv_type_t ctype)
{
  if (clvl<0) {
    return (TSV_ERR_NOTFOUND);
  }
  //printf("colname_set(%d,'%s',%d)\n",clvl,cname.c_str(),cidx); //
  if (clvl>=(int)m_column_map.size()) {
    // plus one as we are going to use 'clvl' as an index...
    m_column_map.resize(clvl+1);
    m_cnametocidx_map.resize(clvl+1);
  }
  // expand the width if needed.
  if (cidx>=(int)m_column_map[clvl].size()) {
    m_column_map[clvl].resize(cidx+1);
  }
  // record info about this column
  m_column_map[clvl][cidx].m_clvl=clvl;
  m_column_map[clvl][cidx].m_cidx=cidx;
  m_column_map[clvl][cidx].m_cname=cname;
  m_column_map[clvl][cidx].m_ctype=ctype;
  // map column name to column idx
  m_cnametocidx_map[clvl][cname]=cidx;

  // default precision if set
  if (m_optPrecision>=0) {
    m_column_map[clvl][cidx].setPrecision(m_optPrecision);
  }

  return (TSV_OK);
}

/// @brief     Look up the cidx of a column
/// @param     clvl      column level at which to look
/// @param     cname     column name
/// @return    level or less than zero on error
int
affx::TsvFile::cname2cidx(int clvl,const std::string& cname,tsv_optionflag_t options)
{
  if ((clvl<0)||(clvl>=(int)m_column_map.size())) {
    return TSV_ERR_NOTFOUND;
  }
  map<string,int>::iterator i;
  i=m_cnametocidx_map[clvl].find(cname);
  // found it?
  if (i!=m_cnametocidx_map[clvl].end()) {
    return ((*i).second);
  }
  // is this a case sensitve match?
  if ((options&affx::TSV_OPT_CASEINSENSTIVE)!=affx::TSV_OPT_CASEINSENSTIVE) {
    // yes... so fail now.
    return TSV_ERR_NOTFOUND;
  }

  // Doing a case-insenstive search is rare, we dont keep an index.
  // Rather, we do a linear search across the keys.
  std::string cname_lc=affx::tolower(cname);
  std::string key_lc;
  for (i=m_cnametocidx_map[clvl].begin();i!=m_cnametocidx_map[clvl].end();i++) {
    key_lc=affx::tolower((*i).first);
    if (key_lc==cname_lc) {
      // found a lowercase match...
      return ((*i).second);
    }
  }

  // isnt there as lowercase either.
  return TSV_ERR_NOTFOUND;
}

// Requres "clvl" and "options" to be defined.
#define CNAME_FIND(_name) { int rv; rv=cname2cidx(clvl,_name,options); if (rv>=0) { return rv; } }

/// @brief     Find a column index by one of its names.
/// @param     clvl      level
/// @param     alias1    first alias to try
/// @param     alias2    second alias to try
/// @return
int
affx::TsvFile::cname2cidx(int clvl,
                          const std::string& alias1,
                          const std::string& alias2,
                          tsv_optionflag_t options)
{
  CNAME_FIND(alias1);
  CNAME_FIND(alias2);
  return TSV_ERR_NOTFOUND;
}
/// @brief     Find a column index by one of its names.
/// @param     clvl      level
/// @param     alias1    first alias to try
/// @param     alias2    second alias to try
/// @param     alias3    third alias to try
/// @return
int
affx::TsvFile::cname2cidx(int clvl,
                          const std::string& alias1,
                          const std::string& alias2,
                          const std::string& alias3,
                          tsv_optionflag_t options)
{
  CNAME_FIND(alias1);
  CNAME_FIND(alias2);
  CNAME_FIND(alias3);
  return TSV_ERR_NOTFOUND;
}
/// @brief     Find a column index by one of its names.
/// @param     clvl      level
/// @param     alias1    first  alias to try
/// @param     alias2    second alias to try
/// @param     alias3    third  alias to try
/// @param     alias4    fourth alias to try
/// @return
int
affx::TsvFile::cname2cidx(int clvl,
                          const std::string& alias1,
                          const std::string& alias2,
                          const std::string& alias3,
                          const std::string& alias4,
                          tsv_optionflag_t options)
{
  CNAME_FIND(alias1);
  CNAME_FIND(alias2);
  CNAME_FIND(alias3);
  CNAME_FIND(alias4);
  return TSV_ERR_NOTFOUND;
}

/// @brief     Covert a column index to a column index
/// @param     clvl      level
/// @param     cidx      index
/// @return    column index
/// @remarks   This exists to make the templates eaiser to write
int
affx::TsvFile::cname2cidx(int clvl,int cidx)
{
  if ((clvl<0)||(clvl>=(int)m_column_map.size())) {
    return TSV_ERR_NOTFOUND;
  }
  return cidx;
}

//////////

/// @brief     Get a pointer to a column given clvl and cidx
/// @param     clvl     level of column
/// @param     cidx     index of column
/// @return    NULL if not found
TsvFileField*
affx::TsvFile::clvlcidx2colptr(int clvl,int cidx) {
  if ((clvl<0)||
      (clvl>=(int)m_column_map.size())||
      (cidx<0)||
      (cidx>=(int)m_column_map[clvl].size())) {
    return NULL;
  }
  return &m_column_map[clvl][cidx];
}

/// @brief     Return a pointer to the column
/// @param     clvl      level of column
/// @param     cname     name of column
/// @return    NULL if not found
TsvFileField*
affx::TsvFile::clvlcidx2colptr(int clvl,const std::string& cname) {
  return clvlcidx2colptr(clvl,cname2cidx(clvl,cname));
}

/// @brief     Get the name of a column by its level and index
/// @param     clvl      column level
/// @param     cidx      column index
/// @param     cname     where to put the name
/// @return    tsv_return_t
int
affx::TsvFile::cidx2cname(int clvl,int cidx,std::string& cname)
{
  TsvFileField* colptr=clvlcidx2colptr(clvl,cidx);
  if (colptr==NULL) {
    return (TSV_ERR_NOTFOUND);
  }
  cname=colptr->m_cname;
  return TSV_OK;
}

std::string
affx::TsvFile::getColumnName(int clvl,int cidx)
{
  std::string cname;
  cidx2cname(clvl,cidx,cname);
  return cname;
}

int
affx::TsvFile::setPrecision(int clvl,int cidx,int places)
{
  TsvFileField* colptr=clvlcidx2colptr(clvl,cidx);
  if (colptr==NULL) {
    return (TSV_ERR_NOTFOUND);
  }
  return colptr->setPrecision(places);
}

int
affx::TsvFile::setPrecision(int clvl,const std::string& cname,int places)
{
  return setPrecision(clvl,cname2cidx(clvl,cname),places);
}

//////////

/// @brief     open a TSV file
/// @param     fname     name of file to open
/// @return    TSV_OK or an error
int
affx::TsvFile::open(const std::string& fname)
{
  int rv;

  close();
  
  // remeber the name
  m_fileName=fname;
  //
  if (m_fileName.empty()) {
    TSV_ERR_ABORT("Cant open an empty filename. (filename='"+m_fileName+"')");
  }

  // should we check to see if this file is wacked out?
  if (m_optCheckFormatOnOpen) {
    if (Fs::isCalvinFile(m_fileName)) {
      TSV_ERR_ABORT("This file is a Calvin file. (filename='"+m_fileName+"')");
      return TSV_ERR_CALVIN;
    }
    if (Fs::isHdf5File(m_fileName)) {
      TSV_ERR_ABORT("This file is an HDF5 file. (filename='"+m_fileName+"')");
      return TSV_ERR_HDF5;
    }
    // test for binary last, as it is a general test.
    // Failing for this text file, commented out for Windows.
    // 
    //26357	15-Dec-2010 19:09:40	FATAL ERROR:TsvFile.cpp:2664: This file appears to be binary. (filename='../../regression-data/data/idata/translation/regression-data/20080109_31set_DMET3_cn_16various.txt')
#ifndef _WIN32    
    if (Fs::isBinaryFile(m_fileName)) {
      TSV_ERR_ABORT("This file appears to be binary. (filename='"+m_fileName+"')");
      return TSV_ERR_BINARY;
    }
#endif
    
  }
  
  // clear before opening -- errors are sticky.
  m_fileStream.clear();
  // hello?!?
  Fs::aptOpen(m_fileStream,fname,std::fstream::in|std::fstream::binary);
  // Check an error opening the file.
  if(!m_fileStream.is_open() || !m_fileStream.good()) {
    if (m_optAbortOnError) {
      TSV_ERR_ABORT("open: Could not open file: '" + fname + "' to read.");
    }
    else {
      return (TSV_ERR_FILEIO);
    }
  }

  // now throw an exception if something really bad happens.
  m_fileStream.exceptions(ios_base::badbit|ios_base::failbit);

#ifdef TSV_USE_RDBUF
  // This appears to be faster than calling ".get()"
  m_rdbuf=m_fileStream.rdbuf();
#endif

  //printf("### opening: '%s' (rdstate=%4d,%s)...\n",
  //       m_fileName.c_str(),
  //       m_fileStream.rdstate(),
  //       (m_fileStream.good()?"good":"not good"));
  //file_buf=m_fileStream.rdbuf();
  if (!m_fileStream.good()) {
    return (TSV_ERR_FILEIO);
  }

  if ( m_rawOpen ) {
    return TSV_OK;
  }
  
  // parse the headers
  f_read_headers();

  // establish the links
  if (m_optLinkVarsOnOpen) {
    rv=formatOk();
    if (rv!=TSV_OK) {
      Verbose::out(1, "TSV: warning: '" + fname + "': bad format (=" + ToStr(rv) + ")");
      //    printf("TSV: warning: '%s': bad format (=%d)\n",fname.c_str(),rv);
    }
  }

  // redudant with above?
  linkvars_maybe();

  return TSV_OK;
}


/// @brief     Open a table file.  Tables dont have column headers
/// @param     fname
/// @return
int
affx::TsvFile::openTable(const std::string& fname)
{
  m_optHasColumnHeader=false;
  m_optAutoColumns=true;
  //
  return open(fname);
}

/// @brief
/// @param     fname
/// @return
int
affx::TsvFile::openCsv(const std::string& fname)
{
  m_optAutoColumns=true;
  m_optAutoDequote=true;
  m_optAutoSenseSep=false; // must be ','
  m_optEscapeChar='\\';    // The escape char is '\'
  m_optEscapeOk=true;
  m_optFieldSep=',';       // set the field sep char
  m_optHasColumnHeader=true;
  //
  return open(fname);
}


/// @brief     Close the TsvFile
int
affx::TsvFile::close()
{
  // close an open stream
  if (m_fileStream.is_open()) {
    m_fileStream.close();
  }
  return TSV_OK;
}

/// @brief Flushs the data to the file.
///        If you are debugging, you might want to explictly flush the output.
///        It is not required. Be warned that this slows down the IO.
void
affx::TsvFile::flush()
{
  if (m_fileStream.is_open()) {
    m_fileStream.flush();
  }
}

/// @brief     Gobble up the tabs without skipping over eol
/// @return    the number of tabs skipped
int
affx::TsvFile::f_advance_tabs()
{
  int c;
  int tabcnt=0;
  int maxtabs=(int)(m_column_map.size()-1);

  m_line_fpos=m_fileStream.tellg();

  // first nibble off expected tabs...
  while (tabcnt<maxtabs) {
    //c=m_fileStream.get();
    c=M_GETC();
    // printf("!%c",c);
    if (c==TSV_CHAR_TAB) {
      tabcnt++;
    }
    else if (c==TSV_CHAR_COMMENT) {
      //m_fileStream.putback((char)c);
      if(tabcnt > 0) {
        TSV_ERR_ABORT("Error in: '" + m_fileName + "'" +
                      " at line " + ToStr(m_lineNum) + ":" +
                      " Not allowed to have comment characters except on first character of line.");
      }
      M_UNGETC(c);
      return TSV_LINE_COMMENT;
    }
    else if ((c==TSV_CHAR_CR)||(c==TSV_CHAR_LF)) {
      //m_fileStream.putback((char)c);
      M_UNGETC(c);
      return (TSV_LINE_BLANK);
    }
    else {
      // m_fileStream.putback((char)c);
      M_UNGETC(c);
      break;
    }
  }

  // skip possibly extra whitespace
  //c=m_fileStream.peek();
  c=M_PEEK();
  if (c==TSV_CHAR_COMMENT) {
    return (TSV_LINE_COMMENT);
  }
  if ((c==TSV_CHAR_CR)||(c==TSV_CHAR_LF)||(c==EOF)) {
    return (TSV_LINE_BLANK);
  }

  if ((c==TSV_CHAR_SPACE)||(c==TSV_CHAR_TAB)) {
    std::fstream::pos_type skipstart;
    int spaceCnt = -1;
    skipstart=m_fileStream.tellg();
    //
    do {
      //c=m_fileStream.get();
      c=M_GETC();
      spaceCnt++;
    } while ((c==TSV_CHAR_SPACE)||(c==TSV_CHAR_TAB));
    // what are we looking at now?
    if (c==TSV_CHAR_COMMENT) {
      //m_fileStream.putback(c);
      if(spaceCnt > 0) {
        TSV_ERR_ABORT("Error in: " + m_fileName + " at line: " + ToStr(m_lineNum) + "."
                      " Not allowed to have comment characters except on first character of line.");
      }
      M_UNGETC(c);
      return (TSV_LINE_COMMENT);
    }
    if ((c==TSV_CHAR_CR)||(c==TSV_CHAR_LF)||(c==EOF)) {
      //m_fileStream.putback(c);
      M_UNGETC(c);
      return (TSV_LINE_BLANK);
    }
    // found a normal char, rewind to start of skip
    m_fileStream.seekg(skipstart);
  }

  return tabcnt;
}

/// @brief     advance over the end of the line (eol)
/// @return    the number of chars read to get there (not including eol chars)
/// @remarks   The eol chars are not included in the count as there could be 1 or 2
int
affx::TsvFile::f_advance_eol()
{
  int charcnt=0;
  int c;

  if (!m_fileStream.good()) {
    return (TSV_ERR_FILEIO);
  }

  /// @todo this is duplicated from getline but we dont want to accum the data so ok?
  while (1) {
    // get a char...
    // c=m_fileStream.get();
    c=M_GETC();
    // eof?
    //if (!m_fileStream.good()) {
    //  break;
    //}
    if (c==EOF) {
      break;
    }
    // LF ends a line
    if (c==TSV_CHAR_LF) {
      break;
    }
    // CR ends a line too.
    if (c==TSV_CHAR_CR) {
      //c=m_fileStream.peek();
      c=M_PEEK();
      // CR could be followed by a LF
      if (c==TSV_CHAR_LF) {
        //c=m_fileStream.get();
        c=M_GETC();
        break;
      }
    }
    // count the chars
    charcnt++;
  }

  // we just went forward a line
  m_lineNum++;

  //
  return charcnt;
}


/// @brief     Read data into a column from the file
/// @param     col       column to fill
/// @return    tsv_return_t
/// @remarks   Dont read past the end of the line.
///            Only use f_advance_eol to do that.
int
affx::TsvFile::f_read_column(TsvFileField* col)
{
  int c;
  //char* buf_ptr;
  //int   buf_len;
  int   bi;
  char  in_quotes;

  //
  assert(col!=NULL);
  // mark this column as being null.
  col->m_isnull=true;
  col->m_value_int_done=false;
  col->m_value_double_done=false;
  col->m_value_uint_done=false;
  col->m_value_ulonglong_done=false;

  // Nothing to be read.
  if (!m_fileStream.good()) {
    col->m_buffer="";
    return (TSV_ERR_FILEIO);
  }

  // Mark it as not being null.
  col->m_isnull=false;

  in_quotes=0;

  // The common case is the field is the same size as last time.
  // Copy the chars into an already existing string.
  bi=0;
  col->m_buffer.resize(0);
  while (1) {
    //c=m_fileStream.get();
    c=M_GETC();

    //
    if ((m_optEscapeChar==c)&&(m_optEscapeOk==true)) {
      //c=m_fileStream.get();
      c=M_GETC();
      c=unescapeChar(c);
    }
    // start or end of quotes?
    else if (((c==m_optQuoteChar1)&&(m_optQuoteChar1!=0))||
             ((c==m_optQuoteChar2)&&(m_optQuoteChar2!=0)))
      {
        if (in_quotes==0) { // open
          in_quotes=c;
        }
        else if (c==in_quotes) { // close
          in_quotes=0;
        }
      }
    else if ((m_optFieldSep==c)&&(in_quotes==0)) {
      col->m_buffer.resize(bi); // trunc the buffer to size.
      break; // discard the field-sep-char
    }
    // end of field? (short value)
    else if ((c==TSV_CHAR_CR)||(c==TSV_CHAR_LF)) {
      //m_fileStream.putback(c); // leave eol char
      M_UNGETC(c);
      col->m_buffer.resize(bi); // trunc
      break;
    }
    // ran out of data
    else if (c==EOF) {
      if (bi==0) {
        col->m_isnull=true; // revise our opinion about this being non-null
      }
      col->m_buffer.resize(bi);
      return (TSV_ERR_FILEIO);
    }

    // add to our buffer
    col->m_buffer.append(1,(char)c); // buffer full, append
    bi++;
  }

  // dequote the column?
  if (col->m_optAutoDequote==true) {
    trim(col->m_buffer);
    dequote(col->m_buffer);
  }

  //printf("z\n"); fflush(NULL);
  return TSV_OK;
}

/// @brief     Read the columns of this level
/// @param     line_clvl column level at which we are reading
/// @return    tsv_return_t
int
affx::TsvFile::f_read_columns(int line_clvl)
{
  assert(line_clvl>=0);

  //clearFieldsBelowClvl(line_clvl+1); // profiling

  if (line_clvl<(int)m_column_map.size()) {
    unsigned int cidx_size=m_column_map[line_clvl].size();
    for (unsigned int cidx=0;cidx<cidx_size;cidx++) {
      f_read_column(&m_column_map[line_clvl][cidx]);
      m_column_map[line_clvl][cidx].linkedvars_assign(this);
      //printf("C:'%s'\n",m_column_map[line_clvl][cidx].buffer.c_str());
    }
  }

  // Read more columns?
  if (m_optAutoColumns) {
    //char colname_buf[100];
    while (!f_lookingat_eol()) {
      int cidx=getColumnCount(line_clvl);
      // create a column
      std::ostringstream stream;
      stream << "col" << cidx;
      //snprintf(colname_buf,sizeof(colname_buf),"col%02d",cidx);
      defineColumn(line_clvl,cidx,stream.str());
      //
      f_read_column(&m_column_map[line_clvl][cidx]);
    }
  }

  // skip past the end of this line...
  f_advance_eol();

  return TSV_OK;
}

std::fstream::pos_type affx::TsvFile::line_fpos()
{
  return m_line_fpos; ///< Where the current line starts
}

//////////

/// @brief     Allocate a new binding object
/// @return    pointer to the allocated binding var
TsvFileBinding*
affx::TsvFile::linkvar_alloc()
{
  TsvFileBinding* var=new TsvFileBinding();
  m_linkvars_vec.push_back(var);
  m_linkvars_done=false;
  return var;
}

/// @brief     clear all the linkages from columns to vars
void
affx::TsvFile::linkvars_clear()
{
  //
  m_linkvars_done=false;
  m_linkvars_errors=0;
  m_linkvars_warnings=0;
  //
  unsigned int m_column_map_size=m_column_map.size();
  for (unsigned int clvl=0;clvl<m_column_map_size;clvl++) {
    unsigned int m_column_map_clvl_size=m_column_map[clvl].size();
    for (unsigned int cidx=0;cidx<m_column_map_clvl_size;cidx++) {
      m_column_map[clvl][cidx].linkedvars_clear();
    }
  }
}

void
affx::TsvFile::linkvars_free()
{
  linkvars_clear();
  for (size_t i=0;i<m_linkvars_vec.size();i++) {
    delete m_linkvars_vec[i];
    m_linkvars_vec[i]=NULL;
  }
  m_linkvars_vec.clear();
}
  
/// @brief     Link a binding to a column
/// @param     clvl      where to link to
/// @param     cidx      where to link to
/// @param     var       the var to link
void
affx::TsvFile::linkvar_column(int clvl,int cidx,TsvFileBinding* var)
{
  var->islinked=false;
  TsvFileField* col=clvlcidx2colptr(clvl,cidx);

  if (col==NULL) {
    // out of bounds
    m_linkvars_errors++;
    return;
  }
  // found the column, copy over info
  col->linkedvar_push(var);
  var->islinked=true;
}

/// @brief     Link a binding var to a column
/// @param     var       var to link
void
affx::TsvFile::linkvar_link(TsvFileBinding* var)
{
  assert(var!=NULL);

  //printf("link=%p errs=%d\n",var,m_linkvars_errors);
  if (var->m_bindto_type==TSV_BINDTO_NONE) {
    // dead
  }
  else if (var->m_bindto_type==TSV_BINDTO_CIDX) { // by index
    linkvar_column(var->m_bindto_clvl,var->m_bindto_cidx,var);
  }
  else if (var->m_bindto_type==TSV_BINDTO_CNAME) { // by name
    int cidx=cname2cidx(var->m_bindto_clvl,var->m_bindto_cname);
    if (cidx>=0) {
      linkvar_column(var->m_bindto_clvl,cidx,var);
    }
  } else {
    TSV_ERR_ABORT("linkvar_link: internal error: m_bindto_type="+ToStr(var->m_bindto_type));
  }

  // check the binding condition...
  if (var->islinked==true) {   // no need to check
    return;
  }
  if ((var->m_flags&TSV_BIND_REQUIRED)!=0) {
    m_linkvars_errors++;
    if (m_optAbortOnError) {
      TSV_ERR_ABORT("Required column: '"+ToStr(var->m_bindto_cname)+"' not found in file: '"+m_fileName+"'.");
    }
    return;
  }
  if ((var->m_flags&TSV_BIND_WARNING)!=0) {
    m_linkvars_warnings++;
    return;
  }
}

/// @brief     Examine the list of bindings and establish the linkages
/// @return    number of unlinked bindings
int
affx::TsvFile::linkvars_makelinks()
{
  // zap the current linkages
  linkvars_clear();
  // walk the vars and link them all
  unsigned int m_linkvars_vec_size=m_linkvars_vec.size();
  for (unsigned int vi=0;vi<m_linkvars_vec_size;vi++) {
    linkvar_link(m_linkvars_vec[vi]);
  }
  //
  m_linkvars_done=true;
  return m_linkvars_errors;
}

/// @brief     Dont do anything if the linkages are done, otherwise link
/// @return    same as linkvars_makelinks
int
affx::TsvFile::linkvars_maybe()
{
  if (m_linkvars_done) {
    return m_linkvars_errors;
  }
  if (!(m_fileStream.is_open())) {
    return 0;
  }
  return linkvars_makelinks();
}

/// @brief     The number of binding errors. (vars which couldnt be linked)
/// @return    same as linkvars_makelinks
int
affx::TsvFile::bindErrorCnt()
{
  // make sure the links are in place.
  linkvars_maybe();
  // report the number of links we couldnt make
  return m_linkvars_errors;
}

/// @brief     Is the format of the file ok
/// @return    tsv_return_t
int
affx::TsvFile::formatOk()
{
  if (bindErrorCnt()==0) {
    return TSV_OK;
  }
  // print the errors
  printBindingErrors();

  return (TSV_ERR_FORMAT);
}

/// @brief     print messages about unbound variables.
void
affx::TsvFile::printBindingErrors()
{
  for (unsigned int i=0;i<m_linkvars_vec.size();i++) {
    if (!m_linkvars_vec[i]->islinked) { // oh no!
      if ((m_linkvars_vec[i]->m_flags&(TSV_BIND_WARNING|TSV_BIND_REQUIRED))!=0) {
        Verbose::out(2, "TSV: warning: did not bind ('" +
                     ToStr(m_linkvars_vec[i]->m_bindto_clvl) + "," +
                     ToStr(m_linkvars_vec[i]->m_bindto_cidx) + ",'" +
                     m_linkvars_vec[i]->m_bindto_cname +"')");
      }
    }
  }
}

/// @brief     Remove all the bindings.
void
affx::TsvFile::unbindAll()
{
  clearBindings();
}

//////////

/// macro to define code
#define TSV_BIND_FUNC(Xtype,Xfield) \
int \
affx::TsvFile::bind(int clvl,int cidx,Xtype* ptr,tsv_bindopt_t flags,int interncache_size) \
{ \
  TsvFileBinding* var=linkvar_alloc(); \
  var->m_bindto_type=TSV_BINDTO_CIDX; \
  var->m_bindto_clvl=clvl; \
  var->m_bindto_cidx=cidx; \
  var->m_flags=flags; \
  var->m_interncache_size=interncache_size; \
  var->Xfield=ptr; \
\
  linkvars_maybe(); \
\
  return TSV_OK; \
} \
int \
affx::TsvFile::bind(int clvl,const std::string& cname,Xtype* ptr,tsv_bindopt_t flags,int interncache_size) \
{ \
  TsvFileBinding* var=linkvar_alloc(); \
  var->m_bindto_type=TSV_BINDTO_CNAME; \
  var->m_bindto_clvl=clvl; \
  var->m_bindto_cname=cname; \
  var->m_flags=flags; \
  var->m_interncache_size=interncache_size; \
  var->Xfield=ptr; \
\
  linkvars_maybe(); \
\
  return TSV_OK; \
}

// Expand the above
TSV_BIND_FUNC(std::string,m_ptr_string);
TSV_BIND_FUNC(int,m_ptr_int);
TSV_BIND_FUNC(double,m_ptr_double);
TSV_BIND_FUNC(float,m_ptr_float);
TSV_BIND_FUNC(unsigned int,m_ptr_uint);
TSV_BIND_FUNC(uint64_t,m_ptr_ulonglong);

// dont need this any more.
#undef TSV_BIND_FUNC

//////////

/// @brief     Rewind back to the start of the data and clear the fields
/// @return    tsv_return_t
int
affx::TsvFile::rewind()
{
  clearFields();
  m_fileStream.clear();
  m_fileStream.seekg(m_fileDataPos);
  //
  m_lineLvl=0;
  m_lineNum=0;
  m_eof=false;
  //
  if (!m_fileStream.good()) {
    m_eof=true;
    return (TSV_ERR_FILEIO);
  }
  return TSV_OK;
}

/// @brief     advance to the next line and bind the values
/// @return    tsv_return_t
int
affx::TsvFile::nextLine()
{
  // insure the vars are inplace
  // we do this after the bind calls
  // linkvars_maybe();

  // some sort of error or eof?
  //if ((!m_fileStream.good())||(m_fileStream.peek()==EOF)) {
  if ((!m_fileStream.good())||(M_PEEK()==EOF)) {
    m_eof=true;
    return (TSV_ERR_FILEIO);
  }

  // insure the vars are inplace
  //linkvars_maybe();

  // count the tabs to figure out what level it is at
  m_lineLvl=f_advance_tabs();
  if (m_lineLvl<0) {
    f_advance_eol();
    return TSV_OK;
  }

  // Read the data into the columns
  return f_read_columns(m_lineLvl);
}

/// @brief     Skip to the next level of data which matches seek_clvl
/// @param     seek_clvl   the level to skip to
/// @return    tsv_return_t
/// @remarks   nextLevel will skip over lines which are more indented than
///            seek_clvl, but will not skip over lines less indented.
int
affx::TsvFile::nextLevel(int seek_clvl)
{
  char c = '\0';
  // insure the vars are inplace
  // linkvars_maybe();

  // Read lines until we find a line of the correct level.
  while (1) {
    if (!m_fileStream.good()) {
      clearFields();
      m_eof=true;
      return (TSV_ERR_FILEIO);
    }
    c = M_PEEK();
    // speed tweak to avoid uncessary seek. Unless this is the 0 level
    // the first character should be a tab. This handles a common case
    // in pgf files.
    if(seek_clvl != 0 && c != TSV_CHAR_TAB && c != TSV_CHAR_COMMENT) {
      return TSV_LEVEL_LAST;
    }
    else if (c == EOF) {
      clearFields();
      return TSV_ERR_EOF;
    }

    // what level is this line at?
    m_lineLvl=f_advance_tabs();

    // skip the line if it isnt a data line.
    if (m_lineLvl<0) {
      f_advance_eol();
      continue;
    }

    // a match; bind and return
    if (m_lineLvl==seek_clvl) {
      return f_read_columns(m_lineLvl);
    }
    // a master; dont skip it
    if (m_lineLvl<seek_clvl) {
      m_fileStream.seekg(m_line_fpos);
      return TSV_LEVEL_LAST;
    }
    // a child; skip it
    if (m_lineLvl>seek_clvl) {
      f_advance_eol();
    }
  }
  return (TSV_ERR_UNKNOWN);
}

//////////



/// @brief     Read a couple of lines of the TsvFile and figure out the types.
/// @return
int
affx::TsvFile::deduce_types()
{
  std::vector<int> clvl_seen;
  int clvl_seen_cnt;
  int clvl_max;
  int clvl;
  int cidx_max;
  int cidx;
  int seen_all_levels=0;

  // start from the beginning.
  rewind();

  //
  clvl_seen.resize(getLevelCount(),0);
  clvl_seen_cnt=0;
  clvl_max=getLevelCount();

  while (nextLine()==affx::TSV_OK) {
    clvl=lineLevel();
    // skip if we have done this level already...
    if (clvl_seen[clvl]==1) {
      continue;
    }
    // mark being seen.
    clvl_seen[clvl]=1;
    clvl_seen_cnt++;

    // scan the columns
    cidx_max=getColumnCount(clvl);
    for (cidx=0;cidx<cidx_max;cidx++) {
      // set type from value
      int tmp_int;
      double tmp_double;
      std::string tmp_string;
      if (get(clvl,cidx,tmp_int)==affx::TSV_OK) {
        set_type(clvl,cidx,affx::TSV_TYPE_INT);
      }
      else if (get(clvl,cidx,tmp_double)==affx::TSV_OK) {
        set_type(clvl,cidx,affx::TSV_TYPE_DOUBLE);
      }
      else if (get(clvl,cidx,tmp_string)==affx::TSV_OK) {
        set_type(clvl,cidx,affx::TSV_TYPE_STRING);
      }
      else {
        set_type(clvl,cidx,affx::TSV_TYPE_UNKNOWN);
      }
    }
      // all levels seen?
    if (clvl_seen_cnt==clvl_max) {
      seen_all_levels=1;
      break;
    }
  }

  // rewind and return
  rewind();
  //
  return ((seen_all_levels==1)?affx::TSV_OK:affx::TSV_ERR_UNKNOWN);
}

int
affx::TsvFile::clear_max_sizes()
{
  // reset the sizes
  for (unsigned int clvl=0;clvl<m_column_map.size();++clvl) {
    for (unsigned int cidx=0;cidx<m_column_map[clvl].size();cidx++) {
      m_column_map[clvl][cidx].m_max_size=0;
    }
  }
  return affx::TSV_OK;
}

int
affx::TsvFile::deduce_sizes()
{
  clear_max_sizes();

  // start from the beginning.
  rewind();

  TsvFileField* col;
  unsigned int clvl;
  //
  while (nextLine()==affx::TSV_OK) {
    clvl=lineLevel();
    // scan the columns
    int cidx_max=getColumnCount(clvl);
    for (int cidx=0;cidx<cidx_max;cidx++) {
      col=&m_column_map[clvl][cidx];
      int col_cur_size=col->m_buffer.size();
      if (col_cur_size>col->m_max_size) {
        col->m_max_size=col_cur_size;
      }
    }
  }

  // rewind and return
  rewind();
  return affx::TSV_OK;
}

/// @brief     Return the declared type of this column
/// @param     clvl
/// @param     cname
/// @return
affx::tsv_type_t
affx::TsvFile::get_type(int clvl,const std::string& cname)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cname);
  if (col==NULL) {
    return (TSV_TYPE_ERR);
  }
  return col->get_type();
}

/// @brief     Return the declared type of this column
/// @param     clvl
/// @param     cidx
/// @return
affx::tsv_type_t
affx::TsvFile::get_type(int clvl,int cidx)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cidx);
  if (col==NULL) {
    return (TSV_TYPE_ERR);
  }
  return col->get_type();
}

/// @brief     Return the declared type of this column
/// @param     clvl
/// @param     cname
/// @return
affx::tsv_type_t
affx::TsvFile::set_type(int clvl,const std::string& cname,affx::tsv_type_t type)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cname);
  if (col==NULL) {
    return (TSV_TYPE_ERR);
  }
  return col->set_type(type);
}

/// @brief     Return the declared type of this column
/// @param     clvl
/// @param     cidx
/// @return
affx::tsv_type_t
affx::TsvFile::set_type(int clvl,int cidx,affx::tsv_type_t type)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cidx);
  if (col==NULL) {
    return (TSV_TYPE_ERR);
  }
  return col->set_type(type);
}

//////////

//! Handy snippet of code.
#define TSV_GET_VALUE_CIDX() \
  TsvFileField* col=clvlcidx2colptr(clvl,cidx); \
  if ((col==NULL)||(col->isNull())) { \
    return (TSV_ERR_NOTFOUND); \
  } \
  return col->get(&val);

/// @brief     Get the string value of a field
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @param     val       The string in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,int cidx,std::string& val)
{
  TSV_GET_VALUE_CIDX();
}

int
affx::TsvFile::get(int clvl,int cidx,short& val)
{
  TSV_GET_VALUE_CIDX();
}

/// @brief     Get the integer value of a field
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @param     val       The integer in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,int cidx,int& val)
{
  TSV_GET_VALUE_CIDX();
}
/// @brief     Get the double value of a field
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @param     val       The double in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,int cidx,double& val)
{
  TSV_GET_VALUE_CIDX();
}

/// @brief     Get the float value of a field
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @param     val       The float in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,int cidx,float& val)
{
  TSV_GET_VALUE_CIDX();
}
/// @brief     Get the unsigned integer value of a field
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @param     val       The integer in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,int cidx,unsigned int& val)
{
  TSV_GET_VALUE_CIDX();
}
/// @brief     Get the unsigned integer value of a field
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @param     val       The integer in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,int cidx,uint64_t& val)
{
  TSV_GET_VALUE_CIDX();
}

#undef TSV_GET_VALUE_CIDX

/// Handy code to get the value of a column
#define TSV_GET_VALUE_CNAME() \
  TsvFileField* col=clvlcidx2colptr(clvl,cname); \
  if ((col==NULL)||(col->isNull())) { \
    return (TSV_ERR_NOTFOUND); \
  } \
  return col->get(&val);

/// @brief     Get the string value of a field
/// @param     clvl      The column level level of field
/// @param     cname     the column name
/// @param     val       The string in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,const std::string& cname,std::string& val)
{
  TSV_GET_VALUE_CNAME();
}

int
affx::TsvFile::get(int clvl,const std::string& cname,short& val)
{
  TSV_GET_VALUE_CNAME();
}

/// @brief     Get the integer value of a field
/// @param     clvl      The column level level of field
/// @param     cname     the column name
/// @param     val       The integer in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,const std::string& cname,int& val)
{
  TSV_GET_VALUE_CNAME();
}
/// @brief     Get the double value of a field
/// @param     clvl      The column level level of field
/// @param     cname     the column name
/// @param     val       The double in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,const std::string& cname,double& val)
{
  TSV_GET_VALUE_CNAME();
}

/// @brief     Get the float value of a field
/// @param     clvl      The column level level of field
/// @param     cname     the column name
/// @param     val       The float in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,const std::string& cname,float& val)
{
  TSV_GET_VALUE_CNAME();
}

/// @brief     Get the unsigned integer value of a field
/// @param     clvl      The column level level of field
/// @param     cname     the column name
/// @param     val       The integer in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,const std::string& cname,unsigned int& val)
{
  TSV_GET_VALUE_CNAME();
}

/// @brief     Get the unsigned integer value of a field
/// @param     clvl      The column level level of field
/// @param     cname     the column name
/// @param     val       The integer in which to store the value
/// @return    a tsv_return_t code
int
affx::TsvFile::get(int clvl,const std::string& cname,uint64_t& val)
{
  TSV_GET_VALUE_CNAME();
}

// cleanup
#undef TSV_GET_VALUE_CNAME

//////////

#define TSV_GET_VEC_BODY()                       \
  TsvFileField* col=clvlcidx2colptr(clvl,cidx);  \
  if ((col==NULL)||(col->isNull())) {            \
    return (TSV_ERR_NOTFOUND);                   \
  }                                              \
  return col->get(vec,sep)

#define TSV_GET_VEC_FUNC(TYPE)                  \
  int                                                                 \
  affx::TsvFile::get(int clvl,int cidx,std::vector<TYPE>* vec,char sep) \
  {                                                                     \
    TSV_GET_VEC_BODY();                                                 \
  }                                                                     \
  int                                                                   \
  affx::TsvFile::get(int clvl,const std::string& cidx,std::vector<TYPE>* vec,char sep) \
  {                                                                     \
    TSV_GET_VEC_BODY();                                                 \
  }

TSV_GET_VEC_FUNC(std::string);
TSV_GET_VEC_FUNC(int);
TSV_GET_VEC_FUNC(float);
TSV_GET_VEC_FUNC(double);

#undef TSV_GET_VEC_BODY
#undef TSV_GET_VEC_FUNC

//////////

/// Handy code to get the value of a column
#define TSV_SET_VALUE_BODY()                    \
  TsvFileField* col=clvlcidx2colptr(clvl,cidx); \
  if (col==NULL) {                              \
    return (TSV_ERR_NOTFOUND);                  \
  }                                             \
  return col->set(val);

#define TSV_SET_VALUE_FUNC(TYPE)                \
  int                                           \
  affx::TsvFile::set(int clvl,int cidx,TYPE val)  \
  {                                               \
    TSV_SET_VALUE_BODY();                         \
  }                                               \
  int                                                                   \
  affx::TsvFile::set(int clvl,const std::string& cidx,TYPE val) \
  {                                                                     \
    TSV_SET_VALUE_BODY();                                               \
  }

//
TSV_SET_VALUE_FUNC(const std::string&)
TSV_SET_VALUE_FUNC(int)
TSV_SET_VALUE_FUNC(unsigned int)
TSV_SET_VALUE_FUNC(float)
TSV_SET_VALUE_FUNC(double)
TSV_SET_VALUE_FUNC(uint64_t)

// dont need it any more.
#undef TSV_SET_VALUE_BODY
#undef TSV_SET_VALUE_FUNC

//// Macros for vectors of data.
#define TSV_SET_VEC_BODY()                      \
  TsvFileField* col=clvlcidx2colptr(clvl,cidx); \
  if (col==NULL) {                              \
    return (TSV_ERR_NOTFOUND);                  \
  }                                             \
  return col->set(vec,sep);

#define TSV_SET_VEC_FUNC(TYPE)                                                \
  int                                                                         \
  affx::TsvFile::set(int clvl,int cidx,const std::vector<TYPE>& vec,char sep) \
  {                                                                     \
    TSV_SET_VEC_BODY()                                                  \
  }                                                                     \
  int                                                                   \
  affx::TsvFile::set(int clvl,const std::string& cidx,const std::vector<TYPE>& vec,char sep) \
  {                                                                     \
    TSV_SET_VEC_BODY()                                                  \
  }

// define them.
TSV_SET_VEC_FUNC(std::string)
TSV_SET_VEC_FUNC(int);
TSV_SET_VEC_FUNC(float);
TSV_SET_VEC_FUNC(double);

#undef TSV_SET_VEC_BODY
#undef TSV_SET_VEC_FUNC

//////////

/// @brief     Is the field set?
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @return    false if set, otherwise true
bool
affx::TsvFile::isNull(int clvl,int cidx)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cidx);
  if (col==NULL) {
    return true;
  }
  return col->isNull();
}
/// @brief     Is the field set?
/// @param     clvl      The column level level of field
/// @param     cname     The column index (0-based) of the field
/// @return    false if set, otherwise true
bool
affx::TsvFile::isNull(int clvl,const std::string& cname)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cname);
  if (col==NULL) {
    return true;
  }
  return col->isNull();
}

/// @brief     Is the field empty (null or no value)?
/// @param     clvl      The column level level of field
/// @param     cidx      The column index (0-based) of the field
/// @return    false if set, otherwise true
bool
affx::TsvFile::isEmpty(int clvl,int cidx)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cidx);
  if (col==NULL) {
    return true;
  }
  return col->isEmpty();
}
/// @brief     Is the field set?
/// @param     clvl      The column level level of field
/// @param     cname     The column index (0-based) of the field
/// @return    false if set, otherwise true
bool
affx::TsvFile::isEmpty(int clvl,const std::string& cname)
{
  TsvFileField* col=clvlcidx2colptr(clvl,cname);
  if (col==NULL) {
    return true;
  }
  return col->isEmpty();
}

////////////////////

/// @brief     Dump the meta-information of the tsvfile
/// @remark    Used for debugging
void
affx::TsvFile::dump()
{
  //
  printf("TsvDump====================\n");
  printf("== tsv: filename='%s'\n",m_fileName.c_str());
  dump_headers();
  dump_bindings();
  printf("====================\n");
}

/// @brief     Dump the header values of the tsvfile
/// @remark    Used for debugging
void
affx::TsvFile::dump_headers()
{
  printf("=== headers:\n");
  affx::TsvFile::header_t* h_ptr;
  for (int hi=0;hi<(int)m_headers_vec.size();hi++) {
    h_ptr=m_headers_vec[hi];
    printf("==header: ");
    if (h_ptr==NULL) {
      printf("NULL\n");
      continue;
    }
    if (h_ptr->m_key=="") {
      printf("%2d:#%s\n",h_ptr->m_order,h_ptr->m_value.c_str());
    } else {
      printf("%2d:#%%%s=%s\n",h_ptr->m_order,h_ptr->m_key.c_str(),h_ptr->m_value.c_str());
    }
  }

  //
  for (unsigned int clvl=0;clvl<m_column_map.size();++clvl) {
    for (unsigned int col=0;col<m_column_map[clvl].size();col++) {
      printf("==map: %3d: %3d : %s\n",clvl,col,m_column_map[clvl][col].m_cname.c_str());
    }
  }
}

/// @brief     Dump the bindings of the tsvfile
/// @remark    Used for debugging
void
affx::TsvFile::dump_bindings()
{
  printf("==bindings:\n");

  for (unsigned int vi=0;vi<m_linkvars_vec.size();vi++) {
    TsvFileBinding* var=m_linkvars_vec[vi];
    printf("%3d : %p : bound to clvl=%2d cidx=%2d cname='%s')\n",
           vi,var,
           var->m_bindto_clvl,var->m_bindto_cidx,var->m_bindto_cname.c_str());
  }
}

/// @brief     Dump the data of the tsvfile
/// @remark    Used for debugging
void
affx::TsvFile::dump_data()
{
  string tmpstr;

  while (nextLine()==TSV_OK) {
    int clvl=lineLevel();
    printf("%02d :",clvl);
    for (int cidx=0;cidx<getColumnCount(clvl);cidx++) {
      if (cidx!=0) {
        //printf("%c",m_optFieldSep);
        printf("|");
      }
      get(clvl,cidx,tmpstr);
      printf("%d='%s'",cidx,tmpstr.c_str());
    }
    printf("\n");
  }
  printf("== Read %6d lines\n",m_lineNum);
}


/// @brief     Dump the indexes
/// @remark    Used for debugging
void
affx::TsvFile::dump_indexes()
{
  printf("indexes ------------------------------\n");
  for (unsigned int i=0;i<m_index_vec.size();i++) {
    TsvFileIndex* idx=m_index_vec[i];
    if (idx!=NULL) {
      idx->dump();
    }
  }
}

////////////////////

/// @brief     Allocate a TsvFileIndex
/// @return    pointer to the new index
TsvFileIndex*
affx::TsvFile::index_alloc()
{
  TsvFileIndex* idx=new TsvFileIndex;
  m_index_vec.push_back(idx);
  // we will need to index
  m_index_done=false;
  return idx;
}

/// @brief     Creates an index object
/// @param     clvl      column level
/// @param     cname     column name
/// @param     kind      TSV_INDEX_STRING or TSV_INDEX_INT
/// @param     flags     unused at the moment
/// @return    TSV_OK
/// @remark    The indexing is not done until it is actually needed
int
affx::TsvFile::defineIndex(int clvl,const std::string& cname,int kind,int flags)
{
  TsvFileIndex* idx=index_alloc();
  idx->m_bindto_type=TSV_BINDTO_CNAME;
  idx->m_bindto_clvl=clvl;
  idx->m_bindto_cname=cname;
  //
  assert(kind!=0);
  idx->m_kind=kind;
  idx->m_flags=flags;
  //
  return TSV_OK;
}

/// @brief     Creates an index object
/// @param     clvl      column level
/// @param     cidx      column
/// @param     kind
/// @param     flags
/// @return
int
affx::TsvFile::defineIndex(int clvl,int cidx,int kind,int flags)
{
  TsvFileIndex* idx=index_alloc();
  idx->m_bindto_type=TSV_BINDTO_CIDX;
  idx->m_bindto_clvl=clvl;
  idx->m_bindto_cidx=cidx;
  //
  assert(kind!=0);
  idx->m_kind=kind;
  idx->m_flags=flags;
  //
  return TSV_OK;
}

/// @brief     Seek to the start of a line but dont read it.
/// @param     line      line to seek to
/// @return    tsv_return_t
int
affx::TsvFile::seekLine(linenum_t line)
{
  //
  indexBuildMaybe();
  // off the end?
  if (line>m_index_linefpos.size()) {
    assert(0);
    return (TSV_ERR_NOTFOUND);
  }
  m_fileStream.clear();
  m_fileStream.seekg(m_index_linefpos[line]);
  // we seeked to this line, but did not read it.
  m_lineNum=line;
  //
  if (!m_fileStream.good()) {
    return (TSV_ERR_FILEIO);
  }
  return TSV_OK;
}

/// @brief     goto a line reading the parent lines and then the line itself
/// @param     line
/// @return tsv_return_t
/// @remarks When we goto a line, we need to goto the parents first
///          to set the parent values. This presents a "read consistent"
///          view of the data.
int
affx::TsvFile::gotoLine(linenum_t line)
{
  vector<linenum_t>::iterator lb;
  linenum_t sline;

  // do some bookkeeping first
  clearFields();
  indexBuildMaybe();
  //printf("gotoLine(%d)===\n",line);

  if (line>=m_index_linefpos.size()) {
    int maxLine = (int)m_index_linefpos.size();
    if (m_optAbortOnError) {
      TSV_ERR_ABORT("Cant goto line " + ToStr(line) + " of " + ToStr(maxLine));
    }
    return (TSV_ERR_NOTFOUND);
  }

  // Goto each parent lines first
  // This will bind the parent values so the view of the data is consistent
  for (unsigned int clvl=0;clvl<m_index_lineclvl.size();clvl++) {
    lb=lower_bound(m_index_lineclvl[clvl].begin(),m_index_lineclvl[clvl].end(),line);
    // Is this a parent line?
    if ((lb==m_index_lineclvl[clvl].end())||(*lb>line)) {
      sline=*(lb-1);
    } else {
      sline=*lb;
    }
    // move and parse
    //printf("%2d : seek(%d)\n",clvl,sline);
    seekLine(sline);
    nextLine();
    // done?
    if (sline==line) {
      break;
    }
  }

  return TSV_OK;
}

int affx::TsvFile::countTotalDataLines()
{
  int line_cnt=0;

  // start at the beginning.
  rewind();
  // count them up.
  while (nextLine()==affx::TSV_OK) {
    // data line levels are >=0
    if (lineLevel()>=0) {
      line_cnt++;
    }
  }
  // back to the beginning so we leave the file at a known state.
  rewind();

  return line_cnt;
}
  

void affx::TsvFile::currentLineAsString(std::string& line)
{
  for (int t=0;t<m_lineLvl;t++) {
    line.append("\t");
  }
  //
  for (size_t c_idx=0;c_idx<m_column_map[m_lineLvl].size();c_idx++) {
    if (c_idx!=0) {
      line.append("\t");
    }
    line.append(m_column_map[m_lineLvl][c_idx].m_buffer);
  }
  line.append("\n");
}

/// @brief     build the indexes for a file
/// @return    tsv_return_t
int
affx::TsvFile::indexBuild()
{
  std::fstream::pos_type excursion_fpos;
  TsvFileIndex* idx;

  //
  m_index_done=false;

  // force building the line index  // DEBUGGING
  //option_lineindex=true;
  // neednt bother building the indexes.
  //if ((option_lineindex==false)&&(m_index_vec.size()==0)) {
  //return TSV_OK;
  //}

  // clear out the line mappings
  m_index_linefpos.clear();
  m_index_lineclvl.resize(m_column_map.size());
  for (unsigned int clvl=0;clvl<m_index_lineclvl.size();clvl++) {
    m_index_lineclvl[clvl].clear();
  }

  // clear out the indexes.
  for (unsigned int i=0;i<m_index_vec.size();i++) {
    idx=m_index_vec[i];
    if (idx!=NULL) {
      idx->clear();
      // pre-compute the cidxes of the indexes which are by name
      if (idx->m_bindto_type==TSV_BINDTO_CNAME) {
        idx->m_bindto_cidx=cname2cidx(idx->m_bindto_clvl,idx->m_bindto_cname);
      }
    }
  }

  // remember where we are before we rewind
  excursion_fpos=m_fileStream.tellg();
  rewind();

  unsigned int m_index_vec_size=m_index_vec.size();

  int l_num;
  while (nextLine()==TSV_OK) {
    l_num=m_lineNum-1; // because nextLine went past it.
    // line->fpos
    m_index_linefpos.push_back(m_line_fpos);
    // if m_lineLvl is <0, then it was a comment or blank line
    // and there no need to do the rest
    //printf("L1: %4d : %4d : %u\n",l_num,m_lineLvl,(unsigned int)m_line_fpos);
    if (m_lineLvl<0) {
      // skip this line
      continue;
    }
    // parent-child lines
    m_index_lineclvl[m_lineLvl].push_back(l_num);
    //std::string key;
    //std::string val;
    //get(0,0,key);
    //get(0,1,val);
    //printf("L2: K:'%s' V:'%s'\n",key.c_str(),val.c_str());

    //
    for (unsigned int i=0;i<m_index_vec_size;i++) {
      TsvFileIndex* idx=m_index_vec[i];
      if ((idx!=NULL)&&
          (idx->m_bindto_clvl==m_lineLvl)&&
          (idx->m_bindto_cidx>=0)) {
        idx->data_add(&m_column_map[m_lineLvl][idx->m_bindto_cidx],l_num);
      }
    }
  }

  //
  m_fileStream.clear();
  m_fileStream.seekg(excursion_fpos);

  // mark the indexes as done
  m_index_done=true;
  for (unsigned int i=0;i<m_index_vec_size;i++) {
    TsvFileIndex* idx=m_index_vec[i];
    if (idx!=NULL) {
      idx->m_done=true;
    }
  }

  return TSV_OK;
}

/// @brief     build the indexes if needed
/// @return    tsv_return_t
int
affx::TsvFile::indexBuildMaybe()
{
  if (m_index_done) {
    return TSV_OK;
  }
  return indexBuild();
}

/// @brief     Find an index which matches what we need
/// @param     clvl   clvl
/// @param     cidx   cidx
/// @param     kind   What is being matched?
/// @return    NULL if no index found
TsvFileIndex*
affx::TsvFile::index_matching(int clvl,int cidx,int kind)
{
  TsvFileIndex* idx;

  for (unsigned int i=0;i<m_index_vec.size();i++) {
    idx=m_index_vec[i];
    if ((idx!=NULL)&&
        (idx->m_bindto_clvl==clvl)&&
        (idx->m_bindto_cidx==cidx)&&
        (idx->m_kind==kind)) {
      return idx;
    }
  }
  return NULL;
}

/// @brief     Do we have a index on this column for strings?
/// @param     clvl   clvl
/// @param     cidx   cidx
/// @param     val    a dummy value for typedness
/// @return    NULL or a pointer to the index
TsvFileIndex*
affx::TsvFile::index_matching_1(int clvl,int cidx,std::string& val)
{
  return index_matching(clvl,cidx,TSV_INDEX_STRING);
}
/// @brief     Do we have a index on this column for ints?
/// @param     clvl   clvl
/// @param     cidx   cidx
/// @param     val    a dummy value for typedness
/// @return    NULL or a pointer to the index
TsvFileIndex*
affx::TsvFile::index_matching_1(int clvl,int cidx,int val)
{
  return index_matching(clvl,cidx,TSV_INDEX_INT);
}
/// @brief     Do we have a index on this column for doubles?
/// @param     clvl   clvl
/// @param     cidx   cidx
/// @param     val    a dummy value for typedness
/// @return    NULL or a pointer to the index
TsvFileIndex*
affx::TsvFile::index_matching_1(int clvl,int cidx,double val)
{
  return index_matching(clvl,cidx,TSV_INDEX_DOUBLE);
}
/// @brief     Do we have a index on this column for unsigned ints?
/// @param     clvl   clvl
/// @param     cidx   cidx
/// @param     val    a dummy value for typedness
/// @return    NULL or a pointer to the index
TsvFileIndex*
affx::TsvFile::index_matching_1(int clvl,int cidx,unsigned int val)
{
  return index_matching(clvl,cidx,TSV_INDEX_UINT);
}
/// @brief     Do we have a index on this column for uint64_t?
/// @param     clvl   clvl
/// @param     cidx   cidx
/// @param     val    a dummy value for typedness
/// @return    NULL or a pointer to the index
TsvFileIndex*
affx::TsvFile::index_matching_1(int clvl,int cidx,uint64_t val)
{
  return index_matching(clvl,cidx,TSV_INDEX_ULONGLONG);
}

/// @brief     Clear the results of the last findBegin
/// @return    tsv_return_t
int
affx::TsvFile::findResultsClear()
{
  m_findresults_pos=0;
  m_findresults.clear();
  return TSV_OK;
}

/// @brief     Handy template
/// @param     clvl   column level
/// @param     cidx   column index
/// @param     op     the kind of comparison to do (tsv_op_t)
/// @param     val    value to search for
/// @param     flags  how to order the results (tsv_orderby_t)
/// @return    tsv_error_t
template<typename T1,typename T2>
int
affx::TsvFile::findBegin_tmpl(int clvl,T1 cidx,int op,T2 val,int flags)
{
  // discard the old
  findResultsClear();

  // gotta be there
  int cidx_int=cname2cidx(clvl,cidx);
  if ((clvl<0)||(cidx_int<0)) {
    return (TSV_ERR_NOTFOUND);
  }

  //
  indexBuildMaybe();

  // which index to use?
  TsvFileIndex* idx=index_matching_1(clvl,cidx_int,val);
  if (idx==NULL) {
    /// @todo if (m_optAutoIndex==true){index_column
    return (TSV_ERR_NOTFOUND);
  }

  //
  //if (idx->done==false) {
  //indexBuild();
  //}

  //
  idx->results_append(m_findresults,op,val);

  // should we sort by line position?
  if ((flags&TSV_ORDERBY_LINE)!=0) {
    sort(m_findresults.begin(),m_findresults.end());
  }

  // dont go to the first result -- findNext will do the movement.
  return TSV_OK;
}

///! Crank out the different types we want to expose to the programmer
#define TSV_DEFUN_FIND_BEGIN(Xtype1,Xtype2) \
int \
affx::TsvFile::findBegin(int clvl,Xtype1 cidx,int op,Xtype2 val,int flags)  \
{ \
  return findBegin_tmpl(clvl,cidx,op,val,flags);    \
}

//! Big list of allowed types.
TSV_DEFUN_FIND_BEGIN(int        ,std::string);
TSV_DEFUN_FIND_BEGIN(std::string,std::string);
TSV_DEFUN_FIND_BEGIN(int        ,int);
TSV_DEFUN_FIND_BEGIN(std::string,int);
TSV_DEFUN_FIND_BEGIN(int        ,double);
TSV_DEFUN_FIND_BEGIN(std::string,double);
TSV_DEFUN_FIND_BEGIN(int        ,unsigned int);
TSV_DEFUN_FIND_BEGIN(std::string,unsigned int);
TSV_DEFUN_FIND_BEGIN(int        ,uint64_t);
TSV_DEFUN_FIND_BEGIN(std::string,uint64_t);

#undef TSV_DEFUN_FIND_BEGIN

//////////

/// @brief     Return the count of found lines.
/// @return    The number of matching lines to the last findBegin.
unsigned int
affx::TsvFile::findResultsCount()
{
  return (int)m_findresults.size();
}

/// @brief     Goto the next result
/// @return    tsv_return_t
int
affx::TsvFile::findNext()
{
  if (m_findresults_pos>=m_findresults.size()) {
    return TSV_FIND_LAST;
  }
  return gotoLine(m_findresults[m_findresults_pos++]);
}


//////////

/// @brief     Define a TSV file from a string of field names
/// @param     definition
/// @return    tsv_return_t
int
affx::TsvFile::defineFile(const std::string& definition)
{
  // clear out the old state
  clear();
  // and define the new...
  return defineFileParse(definition);
}

/// @brief     Overlay the definition on top of the current file.
/// @param     definition
/// @return    tsv_return_t
/// @remarks defineFile uses "\n" to seperate levels, "\t" to seperate columns
///          and "," to add options to the field name.
///          at the moment there arent any options.
int
affx::TsvFile::defineFileParse(const std::string& definition)
{
  std::vector<std::string> split_line;
  std::vector<std::string> split_col;
  std::vector<std::string> split_coldef;

  splitstr(definition,'\n',split_line);
  for (unsigned int li=0;li<split_line.size();li++) {
    if (split_line[li]!="") {
      splitstr(split_line[li],'\t',split_col);
      for (unsigned int ci=0;ci<split_col.size();ci++) {
        // name,options
        splitstr(split_col[ci],',',split_coldef);
        //
        defineColumn(li,ci,split_coldef[0]);
      }
    }
  }
  return TSV_OK;
}

//////////

/// @brief     Write a number of Field seperators to the file
/// @param     cnt
void
affx::TsvFile::writeFieldSep(int cnt)
{
  for (int i=0;i<cnt;i++) {
    m_fileStream << m_optFieldSep;
  }
}

/// @brief     Write a string to the output stream, quoting if needed
/// @param     str
/// @return    tsv_return_t
int
affx::TsvFile::write_str(const std::string& str)
{
  if (m_optDoQuote==true) {
    m_fileStream<<m_optQuoteChar<<str<<m_optQuoteChar;
  } else {
    m_fileStream<<str;
  }
  return TSV_OK;
}

/// @brief     Write the keyval headers to the file
void
affx::TsvFile::writeKeyValHeaders()
{
  int h_size;
  int h_i;
  affx::TsvFile::header_t* h_ptr;

  // want to write them in order.
  resortHeaders();

  h_size=(int)m_headers_vec.size();
  for (h_i=0;h_i<h_size;h_i++) {
    h_ptr=m_headers_vec[h_i];
    if (h_ptr==NULL) {
      continue;
    }
    // Dont output bad headers which got stuck in with "addHeader_nocheck"
    /// @todo Maybe output them all anyways?
    // a comment?
    if (h_ptr->m_key=="") {
      m_fileStream << TSV_STRING_COMMENT << h_ptr->m_value << m_optEndl;
    }
    // key=value
    else if (headerKeyLegal(h_ptr->m_key)==TSV_OK) {
      m_fileStream << TSV_STRING_META << h_ptr->m_key << "=" << h_ptr->m_value << m_optEndl;
    }
  }
}

/// @brief     Write the headers for a given level
/// @param     clvl
void
affx::TsvFile::writeColumnHeaders_clvl(int clvl)
{
  if ((0<=clvl)&&(clvl<m_column_map.size())) {
    size_t cidx_size=m_column_map[clvl].size();
    size_t cidx_size_1=cidx_size-1;
    for (size_t cidx=0;cidx<cidx_size;cidx++) {
      write_str(m_column_map[clvl][cidx].m_cname);
      if (cidx<cidx_size_1) {
        m_fileStream << m_optFieldSep;
      }
    }
  }
  m_fileStream << m_optEndl;
}

/// @brief     Write the "headerN" lines which define the Columns
void
affx::TsvFile::writeColumnHeaders()
{
  for (unsigned int clvl=0;clvl<m_column_map.size();clvl++) {
    m_fileStream << "#%" << m_headName << clvl << "=";
    writeFieldSep(clvl);
    writeColumnHeaders_clvl(clvl);
  }
}

/// @brief     Write the file headers
void
affx::TsvFile::writeHeaders()
{
  writeKeyValHeaders();
  writeColumnHeaders();
}

/// @brief     open the stream for writing
/// @param     filename
/// @return    tsv_return_t
tsv_return_t
affx::TsvFile::writeOpen(const std::string& filename)
{
  // finish off the old stream
  close();
  // clear the error conditions before trying the open
  m_fileStream.clear();
  // start
  m_fileName=filename;

  Fs::aptOpen(m_fileStream, filename,std::ios::out|std::ios::binary);
  // throw if something really bad happens.
  m_fileStream.exceptions(ios_base::badbit|ios_base::failbit);
  //
  if ((!m_fileStream.is_open())||(!m_fileStream.good())) {
    if (m_optAbortOnError) {
      TSV_ERR_ABORT("writeOpen(): Couldn't open file '" + filename + "' to write.");
    }
    //    printf("open of '%s' failed!\n",filename.c_str());
    return (TSV_ERR_FILEIO);
  }
  return TSV_OK;
}

/// @brief     Set the options up for "tsv" format and write the tsv-v1 header
/// @param     filename
/// @return
tsv_return_t
affx::TsvFile::writeTsv_v1(const std::string& filename)
{
  // this file format only supports one level.
  if (getLevelCount()>1) {
    return (TSV_ERR_FORMAT);
  }

  // set the personality
  m_optFieldSep='\t';
  m_optDoQuote=false;
  m_optQuoteChar='"';

  //
  tsv_return_t rv;
  if ((rv=writeOpen(filename))!=TSV_OK) {
    return rv;
  }

  writeKeyValHeaders();
  if (getLevelCount()==1) {
    writeColumnHeaders_clvl(0);
  }

  flush();
  return TSV_OK;
}

/// @brief     Open the file and write the headers
/// @param     filename  filename to open
/// @return    tsv_return_t
/// @remarks   Once open, set the values with "tsv->set(lvl,col,val)" and
///            write them with "tsv->writeLevel(lvl)".
///            If you want to define your own format, you can cobble it up from:
///            writeOpen, setting the options, and the writeHeaders().
tsv_return_t
affx::TsvFile::writeTsv_v2(const std::string& filename)
{
  tsv_return_t rv;
  if ((rv=writeOpen(filename))!=TSV_OK) {
    return rv;
  }

  writeHeaders();

  flush();
  return TSV_OK;
}


/// @brief     open the file for writing in "v2" format.
/// @param     filename
/// @return    tsv_error_t
tsv_return_t
affx::TsvFile::writeTsv(const std::string& filename)
{
  // use v2 ("#%headerN=") when required
  if (getLevelCount()>0) {
    return writeTsv_v2(filename);
  }
  // otherwise v1
  else {
    return writeTsv_v1(filename);
  }
}

/// @brief     Set the options up for "csv" format and write the CSV header
/// @param     filename
/// @return
tsv_return_t
affx::TsvFile::writeCsv(const std::string& filename)
{
  // this file format only supports one level.
  if (getLevelCount()!=1) {
    return (TSV_ERR_FORMAT);
  }

  // set the personality
  m_optFieldSep=',';
  m_optDoQuote=true;
  m_optQuoteChar='"';

  //
  tsv_return_t rv;
  if ((rv=writeOpen(filename))!=TSV_OK) {
    return rv;
  }

  //
  writeKeyValHeaders();
  writeColumnHeaders_clvl(0);

  //
  flush();
  return TSV_OK;
}

/// @brief     Write a level of data to the file.
/// @param     clvl      The level to write
/// @return    tsv_return_t
int
affx::TsvFile::writeLevel(int clvl)
{
  if ((clvl<0)||(clvl>=(int)m_column_map.size())) {
    // no levels to be written.
    assert(0);
    return (TSV_ERR_NOTFOUND);
  }

  // indent for the current level
  writeFieldSep(clvl);

  size_t cidx_size=(int)m_column_map[clvl].size();
  size_t cidx_size_1=cidx_size-1;
  int last_precision=-1;

  for (size_t cidx=0;cidx<cidx_size;cidx++) {
    TsvFileField* col=&m_column_map[clvl][cidx];
    //
    if (m_optDoQuote==true) {
      m_fileStream<<m_optQuoteChar;
    }
    // pick an output method based on the the current format of the value.
    // the ifs are ordered by frequency.
    if (col->m_val_state==affx::VALSTATE_STRING) {
      if ((m_optEscapeOk==true)&&(m_optEscapeChar!=0)) {
        m_fileStream << escapeString(col->m_buffer,m_optEscapeChar);
      }
      else {
        m_fileStream << col->m_buffer;
      }
    }
    else if (col->m_val_state==affx::VALSTATE_DOUBLE) {
      // change in precision?  Avoid doing this each time around.
      if (last_precision!=col->m_precision) {
        m_fileStream.setf(ios::fixed, ios::floatfield);
        m_fileStream.precision(col->m_precision);
        last_precision=col->m_precision;
      }
      m_fileStream << col->m_value_double;
    }
    else if (col->m_val_state==affx::VALSTATE_INT) {
      m_fileStream << col->m_value_int;
    }
    else {
      TSV_ERR_ABORT("writeLevel(): internal error. m_val_state="+ToStr(col->m_val_state));
    }
    //
    if (m_optDoQuote==true) {
      m_fileStream<<m_optQuoteChar;
    }
    //
    if (cidx<cidx_size_1) {
      m_fileStream << m_optFieldSep;
    }
  }
  m_fileStream << m_optEndl;

  //
  if (!m_fileStream.good()) {
    TSV_ERR_ABORT("writeLevel(): bad filestream.");
    return (TSV_ERR_FILEIO);
  }

  return TSV_OK;
}

/// @brief     Copy the "format" of the file
/// @param     f_tsv     the tsv to copy from
void
affx::TsvFile::copyFormat(affx::TsvFile& f_tsv)
{
  clear();
  //
  for (int clvl=0;clvl<f_tsv.getLevelCount();clvl++) {
    for (int cidx=0;cidx<f_tsv.getColumnCount(clvl);cidx++) {
      std::string cname;
      f_tsv.cidx2cname(clvl,cidx,cname);
      defineColumn(clvl,cidx,cname);
    }
  }
}

/// @brief     copy the fields from one tsv to another tsv
/// @param     f_tsv
/// @param     clvl      level at which to copy
/// @return    tsv_return_t
int
affx::TsvFile::copyLevel(affx::TsvFile& f_tsv,int clvl)
{
  if ((clvl<getLevelCount())&&(clvl<f_tsv.getLevelCount())) {
    for (int cidx=0;cidx<f_tsv.getColumnCount(clvl);cidx++) {
      set(clvl,cidx,f_tsv.m_column_map[clvl][cidx].m_buffer);
    }
    return TSV_OK;
  }
  return (TSV_ERR_NOTFOUND);
}

int
affx::TsvFile::extractColToVec(const std::string& fileName,
                               const std::string& colName,
                               std::vector<std::string>* vec,
                               int optEscapeOk)
{
  affx::TsvFile tsv;
  int col_idx;
  std::string tmp_str;

  vec->clear();

  if (tsv.open(fileName)!=TSV_OK) {
    TSV_ERR_ABORT("extractColToVec: Cant open: '" +fileName+"'");
  }
  if (optEscapeOk==0) {
    tsv.m_optEscapeOk=false;
  }

  col_idx=tsv.cname2cidx(0,colName);
  if (col_idx<0) {
    TSV_ERR_ABORT("extractColToVec: column '"+colName+"' not found in file '"+fileName+"'");
  }

  while(tsv.nextLevel(0)==TSV_OK) {
    if (tsv.get(0,col_idx,tmp_str)!=TSV_OK) {
      TSV_ERR_ABORT("extractColToVec: Problem reading '"+colName+"' from '"+fileName+"'");
    }
    vec->push_back(tmp_str);
  }
  tsv.close();
  //
  return TSV_OK;
}

int
affx::TsvFile::extractColToVec(const std::string& fileName,
                               const std::string& colName,
                               std::vector<std::string>* vec)
{
  int optEscapeOk=1;
#ifdef WIN32
  optEscapeOk=0;
#endif
  return extractColToVec(fileName,colName,vec,optEscapeOk);
}

//////////

int
affx::TsvFile::setPrecision(int p)
{
  m_optPrecision=p;
  return TSV_OK;
}

//////////

int
affx::TsvFile::printDuplicateHeaders()
{
  header_iter_t iter,iter_last;
  int dup_cnt=0;

  iter=m_headers_bykey.begin();
  // skip non key=val headers
  while ((iter!=m_headers_bykey.end())&&(iter->second->m_key=="")) {
    iter++;
  }
  if (iter==m_headers_bykey.end()) {
    return 0;
  }
  // walk the keys in order looking for dups
  while (1) {
    iter_last=iter;
    iter++;
    // done?
    if (iter==m_headers_bykey.end()) {
      break;
    }
    // a dup?
    if (iter->second->m_key==iter_last->second->m_key) {
      // print all the dup headers
      iter_last->second->print();
      dup_cnt++;
      while (iter->second->m_key==iter_last->second->m_key) {
        iter->second->print();
        dup_cnt++;
        iter++;
      }
    }
  }
  //
  return dup_cnt;
}

//
affx::tsv_type_t TsvFile::stringToColType(const std::string& str)
{
  if (str=="string") {
    return TSV_TYPE_STRING;
  }
  if (str=="int") {
    return TSV_TYPE_INT;
  }
  if (str=="float") {
    return TSV_TYPE_FLOAT;
  }
  if (str=="double") {
    return TSV_TYPE_DOUBLE;
  }
  // it is an error if we couldnt convert the string.
  return TSV_TYPE_ERR;
}


/// @brief     Count the number of total lines, assuming a text file. 
/// @param     filename     name of file to open
/// @return    Line count. TSV_ERR_FILEIO, TSV_ERR_NOTFOUND on error.

int TsvFile::getLineCountInFile(const std::string& filename, bool abortOnError)
{

  if ( filename.empty() || !Fs::fileExists(filename) ) {
    if (abortOnError) {
      TSV_ERR_ABORT("affx::TsvFile::getLineCountInFile: file not found: '" + filename + "'.");
    }
    else {
      return 0;
    }
  }

  std::ifstream fstream;
  Fs::aptOpen(fstream, filename, std::ios_base::binary);
  
  // Check an error opening the file.
  if(!fstream.is_open() || !fstream.good()) {
    if (abortOnError) {
      TSV_ERR_ABORT("affx::TsvFile::getLineCountInFile: Could not open file: '" + filename + "' to read.");
    }
    else {
      return 0;
    }
  }
  
  int count = TSV_ERR_FILEIO;

  try {
    count = std::count(std::istreambuf_iterator<char>(fstream), std::istreambuf_iterator<char>(), '\n');
    fstream.seekg(-1, std::ios_base::end);
    char last = '\0';
    fstream.get(last);
    if ( last != '\n' ) {
      count++;
    }
  }
  catch (exception e) {
    if ( abortOnError) {
      TSV_ERR_ABORT(std::string("affx::TsvFile::getLineCountInFile: read error '")+strerror(errno)+"'.");
    }
    else {
      fstream.close();
      return 0;
    }
  }
  fstream.close();
  return count;
  
}

/// @brief     Replace a character in a file with another. 
/// @param     filename name of file for replacement
/// @param     a source character
/// @param     b replacement character
/// @return    TSV_OK or (TSV_ERR_FILEIO, TSV_ERR_NOTFOUND) on error.
int TsvFile::replaceCharInFile(const std::string& filename, char a, char b, bool abortOnError )
{

  tsv_return_t rv = TSV_OK;

  if ( filename.empty() || !Fs::fileExists(filename) ) {
    if (abortOnError) {
      TSV_ERR_ABORT("affx::TsvFile::replace: file not found: '" + filename + "'.");
    }
    else {
      return TSV_ERR_NOTFOUND;
    }
  }


  std::string tmp_name = filename + "~";
  
  // Aborts if permission denied
  if ( Fs::touch(tmp_name, abortOnError)  != APT_OK ) {
    return TSV_ERR_FILEIO;
  }

  try {
    std::ifstream in_file(filename.c_str());
    std::ofstream out_file(tmp_name.c_str());
    std::istreambuf_iterator<char> in(in_file);
    std::istreambuf_iterator<char> eos;
    std::ostreambuf_iterator<char> out(out_file);
    std::replace_copy(in, eos, out, a, b);
  }
  catch (exception e) {
    if ( abortOnError) {
      TSV_ERR_ABORT(std::string("affx::TsvFile::replace: error '")+strerror(errno)+"'.");
    }
    else {
      rv = TSV_ERR_FILEIO;
    }
  }
    
  if ( Fs::rm(filename, false) == APT_OK ) {
    if ( !Fs::fileRename(tmp_name, filename, false )) {
      rv = TSV_ERR_FILEIO;
    }
  }
  else{
    Fs::rm(tmp_name, false);
    rv = TSV_ERR_FILEIO;
  }
  
  return rv;
  
}

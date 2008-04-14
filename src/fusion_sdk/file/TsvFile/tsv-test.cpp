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

/*
 * \file   tsv-test.cpp
 * \brief  Tests for TsvFile reading and writing.
 *         This program checks that TsvFile is working as expected.
 */

#include <sstream>
//
#include "file/TsvFile/TsvFile.h"
// include these to check them for parsing errors.
#include "file/TsvFile/BgpFile.h"
#include "file/TsvFile/ClfFile.h"
#include "file/TsvFile/PgfFile.h"
#include "file/TsvFile/SnpTable.h"

//
using namespace std;
using namespace affx;

/// @brief     Check that the tsv->trim methods work.
void
check_trim()
{
  std::string trimstr1=" \t1 2 3\t ";
  std::string trimstr1s="     1 2 3     ";
  std::string trimstr2;

  trimstr2=trimstr1;
  ltrim(trimstr2);
  //printf("ltrim: '%s'\n",trimstr2.c_str());
  assert(trimstr2=="1 2 3\t ");

  trimstr2=trimstr1s;
  rtrim(trimstr2);
  //printf("rtrim: '%s'\n",trimstr2.c_str());
  assert(trimstr2=="     1 2 3");

  trimstr2=trimstr1;
  rtrim(trimstr2);
  //printf("rtrim: '%s'\n",trimstr2.c_str());
  assert(trimstr2==" \t1 2 3");

  trimstr2=trimstr1;
  trim(trimstr2);
  //printf("trim:  '%s'\n",trimstr2.c_str());
  assert(trimstr2=="1 2 3");

  trimstr2="abc";
  trim(trimstr2);
  //printf("trim:  '%s'\n",trimstr2.c_str());
  assert(trimstr2=="abc");

  trimstr2="";
  trim(trimstr2);
  //printf("trim:  '%s'\n",trimstr2.c_str());
  assert(trimstr2=="");
}

/// Check the dequoting of a string.
#define CHECK_DEQUOTE(Xstr1,Xstr2) \
  str1=Xstr1; \
  str2=str1;  \
  dequote(str2); \
  if (0) printf("dequote('%s')=='%s'\n",str1.c_str(),str2.c_str());  \
  assert(str2==Xstr2);

/// @brief     Check that dequoting works as expected
void
check_dequote()
{
  std::string str1;
  std::string str2;

  CHECK_DEQUOTE("abc","abc");
  CHECK_DEQUOTE("'abc'","abc");
  CHECK_DEQUOTE("'abc","'abc");
  CHECK_DEQUOTE("abc'","abc'");
  CHECK_DEQUOTE("''","");
  CHECK_DEQUOTE("\"","\"");
  CHECK_DEQUOTE("\'","\'");
  CHECK_DEQUOTE("\"abc\"","abc");
  CHECK_DEQUOTE("\'abc\"","\'abc\"");
  CHECK_DEQUOTE("\"abc\'","\"abc\'");
}

/// @brief     print the results of splitstr
/// @param     strvec
void
check_splitstr_print(std::vector<std::string>& strvec)
{
  printf("#---\n");
  for (unsigned int i=0;i<strvec.size();++i) {
    printf("%3d:%s\n",i,strvec[i].c_str());
  }
}

/// @brief     Check splitstr works.
void
check_splitstr()
{
  std::vector<std::string> strvec;
  std::string str;

  str="a-b-c"; // ("a","b","c")
  splitstr(str,'-',strvec);
  //check_splitstr_print(strvec);
  assert(strvec.size()==3);

  str=""; // ("")
  splitstr(str,'-',strvec);
  //check_splitstr_print(strvec);
  assert(strvec.size()==1);

  str="-"; // ("","")
  splitstr(str,'-',strvec);
  //check_splitstr_print(strvec);
  assert(strvec.size()==2);
}


/// @brief     Check that the column name functions work.
void
check_field_name()
{
  affx::TsvFile* tsv=new affx::TsvFile();
  std::string cname;
  int cidx;

  cname="abc";
  tsv->defineColumn(0,1,cname);
  cidx=tsv->cname2cidx(0,cname);
  //printf("0:%s=%d\n",cname.c_str(),cidx);
  assert(cidx==1);

  cname="def";
  tsv->defineColumn(0,10,cname);
  cidx=tsv->cname2cidx(0,cname);
  //printf("0:%s=%d\n",cname.c_str(),cidx);
  assert(cidx==10);

  // Make sure the alias finds it
  cidx=tsv->cname2cidx(0,"bogus1",cname);
  assert(cidx==10);
  cidx=tsv->cname2cidx(0,"bogus1","bogus2",cname);
  assert(cidx==10);
  cidx=tsv->cname2cidx(0,"bogus1","bogus2","bogus3",cname);
  assert(cidx==10);

  //
  cidx=tsv->cname2cidx(0,cname,"bogus1");
  assert(cidx==10);
  cidx=tsv->cname2cidx(0,cname,"bogus1","bogus2");
  assert(cidx==10);
  cidx=tsv->cname2cidx(0,cname,"bogus1","bogus2","bogus3");
  assert(cidx==10);

  //
  cname="xyz";
  cidx=tsv->cname2cidx(0,cname);
  assert(cidx==TSV_ERR_NOTFOUND);

  // Make sure the alias dont find it
  cidx=tsv->cname2cidx(0,"bogus1",cname);
  assert(cidx==TSV_ERR_NOTFOUND);
  cidx=tsv->cname2cidx(0,"bogus1","bogus2",cname);
  assert(cidx==TSV_ERR_NOTFOUND);
  cidx=tsv->cname2cidx(0,"bogus1","bogus2","bogus3",cname);
  assert(cidx==TSV_ERR_NOTFOUND);

  //
  delete tsv;
}

/// @brief     Check the conversion of a field buffer
void
check_field_convert()
{
  affx::TsvFileField col;
  double val_double;
  int val_int;
  int rv;

  // a good int conversion
  col.clear();
  col.setBuffer("123");
  rv=col.get(val_int);
  assert((rv==TSV_OK)&&(val_int==123));
  rv=col.get(val_int);
  assert((rv==TSV_OK)&&(val_int==123));

  // a bad int conversion
  col.clear();
  col.setBuffer("abc");
  rv=col.get(val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));
  rv=col.get(val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));

  // a bad int conversion
  col.clear();
  col.setBuffer("123.4");
  rv=col.get(val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));
  rv=col.get(val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));

  // a good double conversion
  col.clear();
  col.setBuffer("123.5");
  rv=col.get(val_double);
  assert((rv==TSV_OK)&&(val_double==123.5));
  rv=col.get(val_double);
  assert((rv==TSV_OK)&&(val_double==123.5));

  // exponent
  col.clear();
  col.setBuffer("123.5e6");
  rv=col.get(val_double);
  assert((rv==TSV_OK)&&(val_double==123.5e6));
  rv=col.get(val_double);
  assert((rv==TSV_OK)&&(val_double==123.5e6));

  // a bad double conversion
  col.clear();
  col.setBuffer("abc");
  rv=col.get(val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));
  rv=col.get(val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));

  // bad partial
  col.setBuffer("123abc");
  rv=col.get(val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));
  rv=col.get(val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));
}

/// @brief     Check that the headers work
void
check_headers()
{
  affx::TsvFile tsv;
  std::string key;
  std::string val;

  // should be empty at start
  tsv.headersBegin();
  while (tsv.headersNext(key,val)==TSV_OK) {
    assert(0);
  }

  // add one and get the value.
  std::string test_str_in="test_val";
  std::string test_str_out;
  tsv.addHeader("test_str_1",test_str_in);
  tsv.getHeader("test_str_1",test_str_out);
  assert(test_str_out==test_str_in);
  // should fail.
  assert(tsv.getHeader("test_str_none",test_str_out)!=TSV_OK);

  int test_int_in=123;
  int test_int_out=0;
  tsv.addHeader("test_int_1",test_int_in);
  tsv.getHeader("test_int_1",test_int_out);
  assert(test_int_out==test_int_in);
  //
  assert(tsv.getHeader("test_int_none",test_int_out)!=TSV_OK);

  double test_double_in=123.456;
  double test_double_out=0;
  tsv.addHeader("test_double_1",test_double_in);
  tsv.getHeader("test_double_1",test_double_out);
  assert(test_double_out==test_double_in);
  // should fail
  assert(tsv.getHeader("test_double_none",test_double_out)!=TSV_OK);

  // They should remain in the order added.
  for (int i=0;i<10;i++) {
    tsv.addHeader("a",i);
    tsv.addHeader("b",i);
    tsv.addHeader("c",i);
    tsv.addHeaderComment("comment");
  }
  //tsv.dump_headers();

  //
  tsv.headersBegin();
  for (int i=0;i<10;i++) {
    tsv.headersFindNext("b",val);
    assert(i==strtol(val.c_str(),NULL,0));
  }

  // no header returned by headersNext should be blank
  tsv.headersBegin();
  while (tsv.headersNext(key,val)==TSV_OK) {
    //printf("#%s=%s\n",key.c_str(),val.c_str());
    assert(key!="");
  }
}

//////////

/// @brief     Check table reading
void
check_read_table_1()
{
  affx::TsvFile tsv;
  int val_ref=0;
  int val_col;

  tsv.openTable("data-table-1.txt");

  while (tsv.nextLevel(0)==TSV_OK) {
    int cidx=0;
    //printf("num=%3d: lvl=%3d:",tsv.lineNumber(),tsv.lineLevel());
    while ((cidx<tsv.getColumnCount(tsv.lineLevel())) &&
           (tsv.get(tsv.lineLevel(),cidx,val_col)==TSV_OK)) {
      //printf("%s(%d=%d)",((cidx==0)?"":"\t"),val_ref,val_col);
      assert(val_col==val_ref);
      val_ref++;
      cidx++;
    }
    //printf("\n");
  }
  tsv.close();
}

//////////

/// @brief     Check the behavior of headers
void
check_headers_1()
{
  affx::TsvFile tsv_a;
  affx::TsvFile tsv_z;
  affx::TsvFile tsv_0;
  std::string key;
  std::string val;
  std::string val_out;
  int val_out_int;
  int i;
  int i_max=10;
  int rv;

  rv=tsv_a.getHeader("No header",val);
  assert(rv==TSV_ERR_NOTFOUND);

  for (i=0;i<i_max;i++) {
    std::ostringstream stream;
    stream << i;
    val=stream.str();
    key="key_"+val;
    tsv_a.addHeader(key,val,i);
    tsv_a.addHeaderComment("123");
    tsv_z.addHeader(key,val,i_max-i-1);
    tsv_0.addHeader(key,val,0);
    tsv_0.addHeaderComment(val);
    //
    rv=tsv_a.getHeader(key,val);
    assert(rv==TSV_OK);
    rv=tsv_a.getHeader(key+"not found",val);
    assert(rv==TSV_ERR_NOTFOUND);
  }
  assert(tsv_a.headerCount()==(2*i_max));

  //
  //tsv_a.dump_headers();
  //tsv_z.dump_headers();
  assert((tsv_z.headerCount())==(1*i_max));
  tsv_z.resortHeaders();
  assert((tsv_z.headerCount())==(1*i_max));
  //tsv_z.dump_headers();

  // should be in the correct order
  for (i=0;i<10;i++) {
    std::ostringstream stream;
    stream << i;
    val=stream.str();
    key="key_"+val;

    rv=tsv_a.getHeader(key,val_out);
    assert(rv==TSV_OK);
    assert(val_out==val);

    rv=tsv_a.getHeader(key,val_out_int);
    assert(rv==TSV_OK);
    assert(val_out_int==i);

    rv=tsv_z.getHeader(key,val_out);
    assert(rv==TSV_OK);
    assert(val_out==val);
  }
  //
  tsv_a.writeTsv("check-a.tsv");
  tsv_a.close();
  tsv_z.writeTsv("check-z.tsv");
  tsv_z.close();
  tsv_0.writeTsv("check-0.tsv");
  tsv_0.close();
}

#define CH2_CNT 10

/// @brief     Check the behavior of headers 
void
check_headers_2()
{
  affx::TsvFile tsv;
  int rv;

  for (int i=0;i<CH2_CNT;i++) {
    tsv.addHeader("abc",i);
  }

  std::string val;
  tsv.headersBegin();
  for (int i=0;i<CH2_CNT;i++) {
    rv=tsv.headersFindNext("abc",val);
    assert(rv==TSV_OK);
  }
  
  rv=tsv.headersFindNext("abc",val);
  assert(rv!=TSV_OK);

  //
  rv=tsv.headersFindNext("def",val);
  assert(rv!=TSV_OK);
  rv=tsv.headersFindNext("abc",val);
  assert(rv==TSV_OK);
}

//////////

/// @brief     Check the reading of a Comma Seperated Value file
void
check_csv_1()
{
  affx::TsvFile tsv;
  int rv;
  std::string tmpstr;
  double tmpdbl;

  //rv=tsv.openCsv("./data-test-2.csv");
  rv=tsv.open("./data-test-2.csv");
  assert(rv==TSV_OK);

  //tsv.dump();

  // The first four lines are all variations of "1,2,3"
  for (int i=0;i<4;i++) {
    rv=tsv.nextLevel(0);    assert(rv==TSV_OK);
    //printf("check_csv_1: line=%3d:",i);
    tsv.get(0,0,tmpstr);
    //printf(" '%s',",tmpstr.c_str());
    //printf("\n");
    rv=tsv.get(0,0,tmpstr); assert((rv==TSV_OK) && (tmpstr=="1"));
    rv=tsv.get(0,0,tmpdbl); assert((rv==TSV_OK) && (tmpdbl==1.0));
    rv=tsv.get(0,1,tmpstr); assert((rv==TSV_OK) && (tmpstr=="2"));
    rv=tsv.get(0,1,tmpdbl); assert((rv==TSV_OK) && (tmpdbl==2.0));
    rv=tsv.get(0,2,tmpstr); assert((rv==TSV_OK) && (tmpstr=="3"));
    rv=tsv.get(0,2,tmpdbl); assert((rv==TSV_OK) && (tmpdbl==3.0));
  }    

  for (int i=0;i<2;i++) {
    rv=tsv.nextLevel(0);    assert(rv==TSV_OK);
    rv=tsv.get(0,0,tmpstr); assert((rv==TSV_OK) && (tmpstr=="\"1\""));
    rv=tsv.get(0,1,tmpstr); assert((rv==TSV_OK) && (tmpstr=="\"2\""));
    rv=tsv.get(0,2,tmpstr); assert((rv==TSV_OK) && (tmpstr=="\"3\""));
  }    

  // a mixed up line ("1",",","3\"")
  rv=tsv.nextLevel(0);    assert(rv==TSV_OK);
  rv=tsv.get(0,0,tmpstr); assert((rv==TSV_OK) && (tmpstr==",1"));
  rv=tsv.get(0,1,tmpstr); assert((rv==TSV_OK) && (tmpstr==","));
  rv=tsv.get(0,2,tmpstr); assert((rv==TSV_OK) && (tmpstr=="\""));

  // escaped field seps. ("1,",",2",3\,4)
  for (int i=0;i<2;i++) {
    rv=tsv.nextLevel(0);    assert(rv==TSV_OK);
    rv=tsv.get(0,0,tmpstr); assert((rv==TSV_OK) && (tmpstr=="1,\""));
    rv=tsv.get(0,1,tmpstr); assert((rv==TSV_OK) && (tmpstr==",2'"));
    rv=tsv.get(0,2,tmpstr); assert((rv==TSV_OK) && (tmpstr=="3,4"));
  }

  // check we are at the end -- move to the next line and get it.
  // # next line is end of file
  rv=tsv.nextLine();
  assert((rv==TSV_OK));
  //assert((tsv.lineLevel()==TSV_LINE_COMMENT));
  // end-of-file
  rv=tsv.nextLine();
  assert((rv==TSV_OK)&&(tsv.lineLevel()==0));
  rv=tsv.get(0,0,tmpstr);
  assert((rv==TSV_OK) && (tmpstr=="end-of-file"));

  tsv.close();
}

/// @brief     Check that next line skips comments
void
check_nextline()
{
  affx::TsvFile tsv;
  int rv;

  rv=tsv.open("./data-test-2.csv");
  assert(rv==TSV_OK);

  while ((rv=tsv.nextLine())==TSV_OK) {
    printf(" %3d : %3d\n",tsv.lineNumber(),tsv.lineLevel());
  }
  tsv.close();
}


/// @brief     Check that indexes work
void
check_index_1()
{
  affx::TsvFile tsv;
  int rv;
  int file_key,file_val;

  rv=tsv.open("./data-test-3.tsv");
  assert(rv==TSV_OK);

  rv=tsv.defineIndex(0,"key",TSV_INDEX_INT,0);
  assert(rv==TSV_OK);

  // negative control
  rv=tsv.findBegin(0,"key",TSV_OP_EQ,999);
  assert(rv==TSV_OK);
  rv=tsv.findNext();
  assert(rv==TSV_FIND_LAST);

  // positive
  for (int key=1;key<10;key++) {
    rv=tsv.findBegin(0,"key",TSV_OP_EQ,key);
    assert(rv==TSV_OK);

    int num_found=0;
    while (tsv.findNext()==TSV_OK) {
      num_found++;
      //
      rv=tsv.get(0,"key",file_key);
      assert(rv==TSV_OK);
      rv=tsv.get(0,"val",file_val);
      assert(rv==TSV_OK);
      //printf(" %2d : %2d => %2d\n",key,file_key,file_val);
      assert(file_key==key);
      assert(file_val==(101*key));
    }
    assert(num_found==1);
  }
  tsv.close();
}

/// @brief     Check that we can read a windows path name
void
check_winpath_1()
{
  int rv;
  std::string pathname;
  affx::TsvFile tsv;
  rv=tsv.open("./data-test-winpath.txt");
  assert(rv==TSV_OK);
  tsv.m_optEscapeOk=false;

  tsv.nextLevel(0);
  rv=tsv.get(0,"pathname",pathname);
  assert(rv==TSV_OK);
  //printf("pathname='%s'\n",pathname.c_str());
  assert(pathname=="C:\\some\\path\\to\\a\\file.txt");
  
  tsv.close();
}

//////////

/// @brief     Main function to call all the check tests
/// @param     argc      count
/// @param     argv      strings
/// @return    error code
int
main(int argc,char* argv[])
{
  // Tests the strings
  check_trim();
  check_splitstr();
  check_dequote();
  //
  check_field_name();
  check_field_convert();
  check_headers();
  //
  check_read_table_1();
  //
  check_headers_1();
  //
  check_csv_1();
  //
  check_index_1();
  //
  check_winpath_1();
  //
  printf("ok.\n");
}

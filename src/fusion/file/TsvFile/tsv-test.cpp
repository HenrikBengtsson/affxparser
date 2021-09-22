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

//
// include these to check them for parsing errors.
#include "file/TsvFile/BgpFile.h"
#include "file/TsvFile/ClfFile.h"
#include "file/TsvFile/PgfFile.h"
#include "file/TsvFile/SnpTable.h"
#include "file/TsvFile/TsvFile.h"
//
#include <sstream>
//

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


void
check_tolower()
{
  assert(affx::tolower("123")=="123");
  assert(affx::tolower("123")!="456");
  //
  assert(affx::tolower("abc")=="abc");
  assert(affx::tolower("abc")!="ABC");
  assert(affx::tolower("ABC")=="abc");
  assert(affx::tolower("DEF")=="def");
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

/// @brief     Check splitstr works.
void
check_countchars()
{
  assert(countchars("",'-')==0);
  assert(countchars("abc",'-')==0);
  assert(countchars("a-b-c",'-')==2);
  assert(countchars("---",'-')==3);
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

void
check_escapeString()
{
  //printf("check_escapeString()\n");

  // negative controls
  assert(escapeChar('A')==0);
  assert(escapeChar('Z')==0);
  assert(escapeChar('a')==0);
  assert(escapeChar('z')==0);
  //
  assert(escapeChar('\n')=='n');
  assert(escapeChar('\r')=='r');
  assert(escapeChar('#')=='#');

  // 
  assert(escapeString("A",'\\')=="A");
  assert(escapeString("Z",'\\')=="Z");
  assert(escapeString("",'\\')=="");
  assert(escapeString("abc",'\\')=="abc");
  // postive
  assert(escapeString("#",'\\')=="\\#");
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

  // check case senseitivty
  cidx=tsv->cname2cidx(0,"ABC");
  assert(cidx==affx::TSV_ERR_NOTFOUND);
  cidx=tsv->cname2cidx(0,"ABC",affx::TSV_OPT_CASEINSENSTIVE);
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

void
check_empty()
{
  affx::TsvFileField col;
  
  //
  col.clear();
  assert(col.isNull()==true);
  assert(col.isEmpty()==true);

  //
  col.setBuffer("");
  assert(col.isNull()==false);
  assert(col.isEmpty()==true);

  //
  col.setBuffer("abc");
  assert(col.isNull()==false);
  assert(col.isEmpty()==false);
}

/// @brief     Check the conversion of a field buffer
void
check_field_convert()
{
  affx::TsvFileField col;
  double val_double;
  int val_int;
  short val_short;
  int rv;

  // a good int conversion
  col.clear();
  col.setBuffer("123");
  rv=col.get(&val_int);
  assert((rv==TSV_OK)&&(val_int==123));
  rv=col.get(&val_int);
  assert((rv==TSV_OK)&&(val_int==123));

  // a bad int conversion
  col.clear();
  col.setBuffer("abc");
  rv=col.get(&val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));
  rv=col.get(&val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));

  // a bad int conversion
  col.clear();
  col.setBuffer("123.4");
  rv=col.get(&val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));
  rv=col.get(&val_int);
  assert((rv==TSV_ERR_CONVERSION)&&(val_int==-1));

  // good short
  col.clear();
  col.setBuffer("456");
  rv=col.get(&val_short);
  assert((rv==TSV_OK)&&(val_short==456));
  rv=col.get(&val_short);
  assert((rv==TSV_OK)&&(val_short==456));

  // bad short
  col.clear();
  col.setBuffer("123456");
  rv=col.get(&val_short);
  assert((rv==TSV_ERR_CONVERSION)&&(val_short==-1));
  rv=col.get(&val_short);
  assert((rv==TSV_ERR_CONVERSION)&&(val_short==-1));

  // bad short but still a good int.
  rv=col.get(&val_int);
  assert((rv==TSV_OK)&&(val_int==123456));

  // a good double conversion
  col.clear();
  col.setBuffer("123.5");
  rv=col.get(&val_double);
  assert((rv==TSV_OK)&&(val_double==123.5));
  rv=col.get(&val_double);
  assert((rv==TSV_OK)&&(val_double==123.5));

  // exponent
  col.clear();
  col.setBuffer("123.5e6");
  rv=col.get(&val_double);
  assert((rv==TSV_OK)&&(val_double==123.5e6));
  rv=col.get(&val_double);
  assert((rv==TSV_OK)&&(val_double==123.5e6));

  // a bad double conversion
  col.clear();
  col.setBuffer("abc");
  rv=col.get(&val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));
  rv=col.get(&val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));

  // bad partial
  col.setBuffer("123abc");
  rv=col.get(&val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));
  rv=col.get(&val_double);
  assert((rv==TSV_ERR_CONVERSION)&&(val_double==-1));


  /*
  for (int p=0;p<5;p++) {
    col.clear();
    col.setPrecision(p);
    col.set(-1.0);
    std::string buf;
    col.get(&buf);
    printf("%d: col val='%s'\n",p,buf.c_str());
  }
  */
}

void
check_field_vec_convert()
{
  affx::TsvFileField col;
  int rv;

#define TEST_VEC_CONVERT_123(TYPE) do {                         \
    std::vector<TYPE> out_vec_##TYPE;                           \
    rv=col.get(&out_vec_##TYPE,',');                            \
    assert((rv==TSV_OK)&&(out_vec_##TYPE.size()==3)&&           \
           (out_vec_##TYPE[0]==123)&&(out_vec_##TYPE[2]==789)); \
  } while(0)

  // a good int conversion
  col.clear();
  col.setBuffer("123,456,789");
  //
  TEST_VEC_CONVERT_123(int);
  TEST_VEC_CONVERT_123(float);
  TEST_VEC_CONVERT_123(double);

  col.setBuffer(" 123 , 456 , 789 ");
  TEST_VEC_CONVERT_123(int);
  TEST_VEC_CONVERT_123(float);
  TEST_VEC_CONVERT_123(double);

  col.setBuffer(" 123 ,, 789 ");
  TEST_VEC_CONVERT_123(int);
  TEST_VEC_CONVERT_123(float);
  TEST_VEC_CONVERT_123(double);

  col.setBuffer(" 123 , , 789 ");
  TEST_VEC_CONVERT_123(int);
  TEST_VEC_CONVERT_123(float);
  TEST_VEC_CONVERT_123(double);

#undef TEST_VEC_CONVERT_123

  //
  std::vector<std::string> out_vec_string;
  col.setBuffer("abc,def,ghi");
  rv=col.get(&out_vec_string);
  assert((rv==TSV_OK)&&
         (out_vec_string.size()==3)&&
         (out_vec_string[0]=="abc")&&
         (out_vec_string[1]=="def")&&
         (out_vec_string[2]=="ghi"));

  //
  col.setBuffer("");
  rv=col.get(&out_vec_string);
  assert((rv==TSV_OK)&&
         (out_vec_string.size()==0));

  col.setBuffer(",");
  rv=col.get(&out_vec_string);
  assert((rv==TSV_OK)&&
         (out_vec_string.size()==2)&&
         (out_vec_string[0]=="")&&
         (out_vec_string[1]==""));

  //
  col.setBuffer("");

#define TEST_VEC_CONVERT_BLANK(TYPE) do {                       \
    std::vector<TYPE> out_vec_##TYPE;                           \
    rv=col.get(&out_vec_##TYPE,',');                            \
    assert((rv==TSV_OK)&&(out_vec_##TYPE.size()==0));           \
  } while (0)

  TEST_VEC_CONVERT_BLANK(int);
  TEST_VEC_CONVERT_BLANK(float);
  TEST_VEC_CONVERT_BLANK(double);

#undef TEST_VEC_CONVERT_BLANK

  std::vector<int> in_vec_int_0;
  //
  col.set("abc");
  //
  in_vec_int_0.clear();
  col.set(in_vec_int_0,',');
  assert(col.m_buffer=="");
  
  std::vector<int> in_vec_int_1(1,123);
  col.set(in_vec_int_1,',');
  assert(col.m_buffer=="123");

  std::vector<int> in_vec_int_2(2,123);
  col.set(in_vec_int_2,',');
  assert(col.m_buffer=="123,123");

  //
  std::vector<std::string> in_vec_str_1;
  col.set(in_vec_str_1,':');
  assert(col.m_buffer=="");
  //
  in_vec_str_1.push_back("abc");
  col.set(in_vec_str_1,':');
  assert(col.m_buffer=="abc");
  //
  in_vec_str_1.push_back("def");   
  in_vec_str_1.push_back("ghi");
  //
  col.set(in_vec_str_1);
  assert(col.m_buffer=="abc,def,ghi");
  col.set(in_vec_str_1,':');
  assert(col.m_buffer=="abc:def:ghi");

  //
  std::vector<double> in_vec_d_1;
  col.setPrecision(6);
  col.set(in_vec_d_1,':');
  assert(col.m_buffer=="");
  //
  in_vec_d_1.push_back(1.0);
  col.set(in_vec_d_1);
  //printf("### m_buffer='%s'\n",col.m_buffer.c_str());
  assert(col.m_buffer=="1.000000");
  //
  in_vec_d_1.push_back(2.0000091);   
  in_vec_d_1.push_back(3.0000011);
  //
  col.set(in_vec_d_1);
  //printf("### m_buffer='%s'\n",col.m_buffer.c_str());
  assert(col.m_buffer=="1.000000,2.000009,3.000001");
}

/// @brief     Check that the headers work
void
check_headers_1()
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

  // test multiple exact matches returned in a vector
  std::vector<std::string> test_vec_out;
  std::string test_str_in_2="test_val_2";
  tsv.addHeader("test_str_2",test_str_in_2);

  tsv.getHeader("test_str_2",test_vec_out);
  assert(test_vec_out.size()==1 && test_vec_out[0]==test_str_in_2);
  assert(tsv.getHeader("test_str_none",test_vec_out)!=TSV_OK);
  assert(test_vec_out.size()==0);
  tsv.getHeader("test_str_1",test_vec_out);
  assert(test_vec_out.size()==1 && test_vec_out[0]==test_str_in);

  tsv.getHeaderAppend("test_str_2",test_vec_out);
  assert(test_vec_out.size()==2 && test_vec_out[1]==test_str_in_2);
  assert(tsv.getHeaderAppend("test_str_none",test_vec_out)!=TSV_OK);
  assert(test_vec_out.size()==2);
  tsv.getHeaderAppend("test_str_1",test_vec_out);
  assert(test_vec_out.size()==3 && test_vec_out[2]==test_str_in);

  // test multiple exact matches returned in a vector
  tsv.getHeaderMatchingKeySubstr("test_str_2",test_vec_out);
  assert(test_vec_out.size()==1 && test_vec_out[0]==test_str_in_2);
  assert(tsv.getHeaderMatchingKeySubstr("test_str_none",test_vec_out)!=TSV_OK);
  assert(test_vec_out.size()==0);
  tsv.getHeaderMatchingKeySubstr("test_str_1",test_vec_out);
  assert(test_vec_out.size()==1 && test_vec_out[0]==test_str_in);

  tsv.getHeaderMatchingKeySubstrAppend("test_str_2",test_vec_out);
  assert(test_vec_out.size()==2 && test_vec_out[1]==test_str_in_2);
  assert(tsv.getHeaderMatchingKeySubstrAppend("test_str_none",test_vec_out)!=TSV_OK);
  assert(test_vec_out.size()==2);
  tsv.getHeaderMatchingKeySubstrAppend("test_str_1",test_vec_out);
  assert(test_vec_out.size()==3 && test_vec_out[2]==test_str_in);

  // test multiple partial matches returned in a vector
  tsv.getHeaderMatchingKeySubstr("test",test_vec_out);
  assert(test_vec_out.size()==2);
  assert(tsv.getHeaderMatchingKeySubstr("test_str_none",test_vec_out)!=TSV_OK);
  assert(test_vec_out.size()==0);
  tsv.getHeaderMatchingKeySubstr("str_1",test_vec_out);
  assert(test_vec_out.size()==1 && test_vec_out[0]==test_str_in);

  tsv.getHeaderMatchingKeySubstrAppend("test",test_vec_out);
  assert(test_vec_out.size()==3);
  assert(tsv.getHeaderMatchingKeySubstrAppend("test_str_none",test_vec_out)!=TSV_OK);
  assert(test_vec_out.size()==3);
  tsv.getHeaderMatchingKeySubstrAppend("str_1",test_vec_out);
  assert(test_vec_out.size()==4 && test_vec_out[3]==test_str_in);

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
  assert(tsv.hasHeaderEqualTo("a","1")==TSV_OK);
  assert(tsv.hasHeaderEqualTo("c","1")==TSV_OK);
  //
  assert(tsv.hasHeaderEqualTo("a","99")==TSV_ERR_NOTFOUND);
  assert(tsv.hasHeaderEqualTo("d","1")==TSV_ERR_NOTFOUND);

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

  assert(tsv.countTotalDataLines()==10);

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
check_headers_2()
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

void
check_comment_1()
{
  affx::TsvFile tsv;
  std::string filename="./check-comment.tsv";
  std::string comment="comment for testing";

  //
  assert(tsv.writeTsv(filename)==affx::TSV_OK);
  assert(tsv.writeFileComment(comment)==affx::TSV_OK);
  tsv.close();
  //
  tsv.open(filename);
  tsv.headersBegin();
  TsvFileHeaderLine* hdr;
  int hdr_cnt=0;
  int hdr_found=0;
  while ((hdr=tsv.nextHeaderPtr())!=NULL) {
    // printf("hdr='%s'\n",hdr->m_value.c_str());
    hdr_cnt++;
    if ((hdr->m_key=="") && (hdr->m_value==comment)) {
      hdr_found++;
    }
  }
  //
  assert((hdr_cnt==1)&&(hdr_found==1));
  //
  tsv.close();
}

#define CH2_CNT 10

/// @brief     Check the behavior of headers
void
check_headers_3()
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

void
check_headers_4()
{
  affx::TsvFile tsv;
  int rv;

  // add some headers "abc" and "ghi" are bookends.
  tsv.addHeader("abc",0);
  for (int i=0;i<5;i++) {
    tsv.addHeader("def",i);
  }
  tsv.addHeader("ghi",11);

  //tsv.dump_headers();

  // get rid of the middle ones.
  tsv.deleteHeaders("def");
  //tsv.dump_headers();
  rv=tsv.headersCount();
  assert(rv==2);
  // tsv.dump_headers();
  
  // shouldnt crash
  tsv.deleteHeaders("def");
  rv=tsv.headersCount();
  assert(rv==2);

  //
  tsv.deleteHeaders("abc");
  rv=tsv.headersCount();
  assert(rv==1);

  //
  tsv.deleteHeaders("ghi");
  rv=tsv.headersCount();
  assert(rv==0);
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

  //printf("check_csv_1: lines=%d\n",tsv.countTotalDataLines());
  assert(tsv.countTotalDataLines()==10);

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
    //
    assert(tsv.isNull(0,2)==false);
    assert(tsv.isNull(0,"header-03")==false);
    assert(tsv.isEmpty(0,2)==false);
    assert(tsv.isEmpty(0,"header-03")==false);
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

void check_read_vec_1()
{
  int rv;
  std::vector<int> vec_i;

  affx::TsvFile tsv;
  rv=tsv.open("./data-test-4.tsv");
  assert(rv==TSV_OK);

  tsv.nextLevel(0);

  rv=tsv.get(0,"col_00",&vec_i);
  assert((rv==TSV_OK)&&(vec_i.size()==3)&&(vec_i[0]==1)&&(vec_i[2]==3));
  
  rv=tsv.get(0,"col_01",&vec_i);
  assert((rv==TSV_OK)&&(vec_i.size()==3)&&(vec_i[0]==4)&&(vec_i[2]==6));

  tsv.nextLevel(0);

  rv=tsv.get(0,"col_00",&vec_i);
  assert((rv==TSV_OK)&&(vec_i.size()==3)&&(vec_i[0]==7)&&(vec_i[2]==9));
  
  rv=tsv.get(0,"col_01",&vec_i,':');
  assert((rv==TSV_OK)&&(vec_i.size()==3)&&(vec_i[0]==10)&&(vec_i[2]==12));

  tsv.nextLevel(0);
  std::string tmp_str;
  std::vector<double> tmp_vec_d;
  
  rv=tsv.get(0,0,tmp_str);
  assert((rv==TSV_OK)&&(tmp_str=="7.005,8.123,9.100"));
  tsv.get(0,0,&tmp_vec_d);
  assert((rv==TSV_OK)&&(tmp_vec_d.size()==3)&&
         (tmp_vec_d[0]==7.005)&&(tmp_vec_d[1]==8.123)&&(tmp_vec_d[2]==9.100));;

  rv=tsv.get(0,"col_01",tmp_str);
  assert((rv==TSV_OK)&&(tmp_str=="7.005005:8.123456:9.100000"));
  rv=tsv.get(0,"col_01",&tmp_vec_d,':');
  assert((rv==TSV_OK)&&(tmp_vec_d.size()==3)&&
         (tmp_vec_d[0]==7.005005)&&(tmp_vec_d[1]==8.123456)&&(tmp_vec_d[2]==9.100000));;

  tsv.close();

}

void check_write_10()
{
  affx::TsvFile tsv;
  tsv.defineColumn(0,0,"col_00");
  tsv.writeTsv_v1("./data-test-10.out");
  for (int i=0;i<10;i++) {
    tsv.set(0,0,i);
    tsv.writeLevel(0);
    // if you are debugging, you might want to explictly flush the output.
    // it is not required. Be warned that this slows down the IO.
    tsv.flush();
  }
  tsv.close();
}

void check_write_vec_1()
{
  int rv;

  affx::TsvFile tsv;
  tsv.defineColumn(0,0,"col_00");
  tsv.defineColumn(0,1,"col_01");
  rv=tsv.writeTsv_v1("./data-test-4.out");

  //
  std::vector<int> vec_i;
  //
  vec_i.clear();
  vec_i.push_back(1); vec_i.push_back(2); vec_i.push_back(3);
  tsv.set(0,0,vec_i);
  vec_i.clear();
  vec_i.push_back(4); vec_i.push_back(5); vec_i.push_back(6);
  tsv.set(0,"col_01",vec_i);
  //
  tsv.writeLevel(0);

  //
  vec_i.clear();
  vec_i.push_back(7); vec_i.push_back(8); vec_i.push_back(9);
  tsv.set(0,0,vec_i);
  vec_i.clear();
  vec_i.push_back(10); vec_i.push_back(11); vec_i.push_back(12);
  tsv.set(0,"col_01",vec_i,':');
  //
  tsv.writeLevel(0);

  //
  std::vector<double> vec_d;
  vec_d.push_back(7.0050051);
  vec_d.push_back(8.123456);
  vec_d.push_back(9.1);  
  //
  tsv.setPrecision(0,0,3);
  tsv.set(0,0,vec_d);
  //
  tsv.setPrecision(0,"col_01",6);
  tsv.set(0,"col_01",vec_d,':');
  //
  tsv.writeLevel(0);

  //
  tsv.close();
}

void check_read_colvec_1()
{
  std::vector<std::string> colvec;
  affx::TsvFile::extractColToVec("data-test-3.tsv","val",&colvec,0);
  //
  if (0) {
    for (int i=0;i<colvec.size();i++) {
      printf("%2d: %s",i,colvec[i].c_str());
    }
  }
  assert(colvec.size()==9);
  assert(colvec[0]=="101");
  assert(colvec[3]=="404");
  assert(colvec[8]=="909");
}

void check_read_pgf_1()
{
  affx::PgfFile pgf;
  std::string pgf_file="data-test-1.pgf";

  pgf.open(pgf_file);
  pgf.rewind();
  
  int probe_cnt=0;
  while (pgf.next_probeset()==affx::TSV_OK) {
    while (pgf.next_atom()==affx::TSV_OK) {
      while (pgf.next_probe()==affx::TSV_OK) {
        probe_cnt++;
      }
    }
  }
  printf("Pgf file '%s' has %d probes.\n",pgf_file.c_str(),probe_cnt);

  pgf.close();
}

void check_line_count()
{

  
  assert(affx::TsvFile::getLineCountInFile("no_eol_test.txt")==1);
  assert(affx::TsvFile::getLineCountInFile("data-table-1.txt")==12);
  assert(affx::TsvFile::getLineCountInFile("data-table-1.txt",true)==12);
  assert(affx::TsvFile::getLineCountInFile("", false)==0);
  assert(affx::TsvFile::getLineCountInFile("bogus43Fzx",false)==0);

  // Tests preserved from  util/CPPTest/UtilTest.cpp

#ifdef _WIN32
  assert(affx::TsvFile::getLineCountInFile("input\\t.e_s#-$tc\\test.txt", false)==0);
  assert(affx::TsvFile::getLineCountInFile("input\\1lqcdfpsi_win.asdf") == 7238);
  assert(affx::TsvFile::getLineCountInFile("input/1lqcdfpsi_win.asdf") == 7238);
  assert(affx::TsvFile::getLineCountInFile("input/1lqcdfpsi_bin.bcdf") == 345 );
#else
  assert(affx::TsvFile::getLineCountInFile("input/t.e_s#-$tc/test.txt", false)==0);
  assert(affx::TsvFile::getLineCountInFile("input/1lqcdfpsi_unix.asdf", false) == 7238);
  assert(affx::TsvFile::getLineCountInFile("input/1lqcdfpsi_bin.bcdf", false) == 345);
#endif
  
}

void check_replace()
{

  assert(affx::TsvFile::replaceCharInFile("data-table-1.txt", '\n', '^', false)==affx::TSV_OK);
  assert(affx::TsvFile::getLineCountInFile("data-table-1.txt")==1);
  

  assert(affx::TsvFile::replaceCharInFile("data-table-1.txt", '^', '\n', false)==affx::TSV_OK);
  assert(affx::TsvFile::getLineCountInFile("data-table-1.txt")==12);

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
  check_tolower();
  check_countchars();
  check_splitstr();
  check_dequote();
  check_escapeString();
  //
  check_field_name();
  check_empty();
  check_field_convert();
  check_field_vec_convert();
  //
  check_headers_1();
  check_headers_2();
  check_headers_3();
  check_headers_4();
  //
  check_read_table_1();
  //
  check_csv_1();
  //
  check_index_1();
  //
  check_winpath_1();
  //
  check_comment_1();
  //
  check_write_10();
  //
  check_read_vec_1();
  check_write_vec_1();
  //
  check_read_colvec_1();
  //
  check_read_pgf_1();
  //
  check_line_count();
  //
  check_replace();
  //
  printf("ok.\n");
}

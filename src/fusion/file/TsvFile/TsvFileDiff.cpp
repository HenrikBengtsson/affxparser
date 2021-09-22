////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008 Affymetrix, Inc.
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

// 
// affy/sdk/file/TsvFile/TsvFileDiff.cpp ---
// 
// $Id: TsvFileDiff.cpp,v 1.3 2009-09-18 03:37:25 mspald Exp $
// 

//
#include "file/TsvFile/TsvFileDiff.h"
//
#include "util/Err.h"
#include "util/Guid.h"
//
#include <cmath>
//

//////////

affx::TsvFileDiff::TsvFileDiff()
{
  //
  //m_opt_print_linenums=true;
  m_opt_print_linenums=false;
  m_opt_print_same=true;
  //
  m_opt_do_headers=true;
  m_opt_do_data=true;
  //
  m_opt_print_max=0;
  m_opt_max_diff=0.0;
  m_opt_print_format=TsvFileDiff::FMT_2;
  //
  m_residuals_filename="";
  m_residuals_tsv=NULL;
  //
  clear();
}

affx::TsvFileDiff::~TsvFileDiff()
{
  clear();
}

void 
affx::TsvFileDiff::clear()
{
  m_diff_cnt=0;
  m_print_cnt=0;
  close_residuals();
}

/////

void 
affx::TsvFileDiff::open_residuals(const std::string& filename,
                                  affx::TsvFile& tsv1,
                                  affx::TsvFile& tsv2)
{
  // no file
  if (filename=="") {
    return;
  }
  //
  m_residuals_tsv=new affx::TsvFile();
  //
  for (int clvl=0;clvl<tsv1.getLevelCount();clvl++) {
    for (int cidx=0;cidx<tsv1.getColumnCount(clvl);cidx++) {
      std::string cname=tsv1.getColumnName(clvl,cidx); // +"_res";
      m_residuals_tsv->defineColumn(clvl,cidx,cname);
    }
  }
  //
  m_residuals_tsv->addHeader("apt-file-type","tsv-diff-residuals");
  m_residuals_tsv->addHeader("tsv-diff-input-file1",tsv1.getFileName());
  m_residuals_tsv->addHeader("tsv-diff-input-file2",tsv2.getFileName());
  m_residuals_tsv->addHeader("file-guid",affxutil::Guid::GenerateNewGuid());
  //
  if (m_residuals_tsv->getLevelCount()==1) {
    m_residuals_tsv->writeTsv_v1(filename);
  } else {
    m_residuals_tsv->writeTsv_v2(filename);
  }
}

void 
affx::TsvFileDiff::close_residuals()
{
  if (m_residuals_tsv!=NULL) {
    m_residuals_tsv->close();
    delete m_residuals_tsv;
    m_residuals_tsv=NULL;
  }
}

/////

int
affx::TsvFileDiff::readOptionsFrom(const std::string& filename)
{
  affx::TsvFile tsv;
  if (tsv.open(filename)!=affx::TSV_OK) {
    Err::errAbort("Cant read TsvFileDiff options from '"+filename+"'");
  }
  readOptionsFrom(tsv);
  tsv.close();
  //
  return 0;
}

int
affx::TsvFileDiff::readOptionsFrom(affx::TsvFile& tsv)
{
  // put option processing here
  return 0;
}

//////////

#define P_LINENUMS_WIDTH "3"

void
affx::TsvFileDiff::p_linenums(affx::TsvFile& tsv1,affx::TsvFile& tsv2)
{
  p_linenums(tsv1.lineNumber(),tsv2.lineNumber());
}

void
affx::TsvFileDiff::p_linenums(int l1,int l2)
{
  //
  if (m_opt_print_linenums) {
    if (l1>=0) {
      printf("%" P_LINENUMS_WIDTH "d",l1);
    }
    else {
      printf("%" P_LINENUMS_WIDTH "s","");
    }
    printf(":");
    if (l2>=0) {
      printf("%" P_LINENUMS_WIDTH "d",l2);
    }
    else {
      printf("%" P_LINENUMS_WIDTH "s","");
    }
    printf(" ");
  }
}

void affx::TsvFileDiff::p_line(affx::TsvFile& tsv) {
  int clvl=tsv.lineLevel();
  int cidx_max=tsv.getColumnCount(clvl);
  std::string val;
  //
  for (int i=0;i<clvl;i++) {
    printf("\t");
  }
  //
  for (int cidx=0;cidx<cidx_max;cidx++) {
    if (cidx!=0) {
      printf("\t");
    }
    printf("\t");
    tsv.get(clvl,cidx,val);
    printf("%s",val.c_str());
  }
  printf("\n");
}

bool
affx::TsvFileDiff::p_inc() {
  m_print_cnt++;
  //
  if ((m_opt_print_max==-1)||(m_print_cnt<m_opt_print_max)) {
    return true;
  }
  else {
    return false;
  }
}

void
affx::TsvFileDiff::p_diff_1line(affx::TsvFile& tsv1, affx::TsvFile& tsv2)
{
  std::string val1,val2;
  int tsv1_clvl=tsv1.lineLevel();
  int tsv1_cidx_max=tsv1.getColumnCount(tsv1_clvl);

  //
  p_linenums(tsv1,tsv2);
  printf("!");
  //
  for (int cidx=0;cidx<tsv1_cidx_max;cidx++) {
    if (cidx!=0) {
      printf("\t");
    }
    printf("\t");
    tsv1.get(tsv1_clvl,cidx,val1);
    tsv2.get(tsv1_clvl,cidx,val2);
    if (val1==val2) {
      printf("%s",val1.c_str());
    }
    else {
      printf("'%s'/'%s'",val1.c_str(),val2.c_str());
    }
  }
  printf("\n");
}

void
affx::TsvFileDiff::p_diff_2line(affx::TsvFile& tsv1, affx::TsvFile& tsv2)
{
  p_linenums(tsv1,tsv2);
  printf("-");
  p_line(tsv1);
  p_linenums(tsv1,tsv2);
  printf("+");
  p_line(tsv2);
}

//////////

int
affx::TsvFileDiff::diff(const std::string& filename1,const std::string& filename2)
{
  clear();

  affx::TsvFile tsv1;
  affx::TsvFile tsv2;

  if (tsv1.open(filename1)!=affx::TSV_OK) {
    return 1;
  }
  if (tsv2.open(filename2)!=affx::TSV_OK) {
    return 2;
  }
  
  int rv=diff(tsv1,tsv2);

  tsv1.close();
  tsv2.close();

  return rv;
}

// file1 + diff => file2

int
affx::TsvFileDiff::diffHeaders(affx::TsvFile& tsv1, affx::TsvFile& tsv2)
{
  int start_diff_cnt=m_diff_cnt;
  std::string key1,val1,key2,val2;

  // do the diff in three passes.

  // 1 = changed values
  tsv1.headersBegin();
  while (tsv1.headersNext(key1,val1)==affx::TSV_OK) {
    if (tsv2.getHeader(key1,val2)==affx::TSV_OK) {
      if (val1==val2) {
        if (m_opt_print_same) {
          if (p_inc()) {
            printf(" #%%%s=%s\n",key1.c_str(),val1.c_str());
          }
        }
      }
      else { // !=
        m_diff_cnt++;
        if (p_inc()) {
          printf("-#%%%s=%s\n",key1.c_str(),val1.c_str());
          printf("+#%%%s=%s\n",key1.c_str(),val2.c_str());
        }
      }
    }
  }

  // 2 = headers in tsv1 not in tsv2
  tsv1.headersBegin();
  while (tsv1.headersNext(key1,val1)==affx::TSV_OK) {
    if (tsv2.getHeader(key1,val2)!=affx::TSV_OK) {
      m_diff_cnt++;
      if (p_inc()) {
        printf("-#%%%s=%s\n",key1.c_str(),val1.c_str());
      }
    }
  }
  // 3 = headers in tsv2 not in tsv1
  tsv2.headersBegin();
  while (tsv2.headersNext(key2,val2)==affx::TSV_OK) {
    if (tsv1.getHeader(key2,val1)!=affx::TSV_OK) {
      m_diff_cnt++;
      if (p_inc()) {
        printf("+#%%%s=%s\n",key2.c_str(),val2.c_str());
      }
    }
  }
  //
  return m_diff_cnt-start_diff_cnt;
}

int
affx::TsvFileDiff::diffData(affx::TsvFile& tsv1, affx::TsvFile& tsv2)
{
  int start_diff_cnt=m_diff_cnt;
  std::string val1,val2;
    
  //
  open_residuals(m_residuals_filename,tsv1,tsv2);

  //
  tsv1.rewind();
  tsv2.rewind();

  //
  tsv1.nextLine();
  tsv2.nextLine();

  //
  while ((!tsv1.eof())||(!tsv2.eof())) {
    //
    int tsv1_clvl=tsv1.lineLevel();
    int tsv2_clvl=tsv2.lineLevel();
    //
    while (((tsv1_clvl>tsv2_clvl)||tsv2.eof())&&(!tsv1.eof())) {
      if (p_inc()) {
        p_linenums(tsv1,tsv2);
        printf("-");
        p_line(tsv1);
      }
      m_diff_cnt+=tsv1.getColumnCount(tsv1_clvl);
      tsv1.nextLine();
      tsv1_clvl=tsv1.lineLevel();
    }
    //
    while (((tsv2_clvl>tsv1_clvl)||tsv1.eof())&&(!tsv2.eof())) {
      if (p_inc()) {
        p_linenums(tsv1,tsv2);
        printf("+");
        p_line(tsv2);
      }
      m_diff_cnt+=tsv2.getColumnCount(tsv2_clvl);
      tsv2.nextLine();
      tsv2_clvl=tsv2.lineLevel();
    }
    //
    if ((tsv1_clvl>=0)&&(tsv2_clvl>=0)) {
      int tsv1_cidx_max=tsv1.getColumnCount(tsv1_clvl);
      int tsv2_cidx_max=tsv2.getColumnCount(tsv2_clvl);
      //
      int cidx_max;
      if (tsv1_cidx_max<tsv2_cidx_max) {
        cidx_max=tsv2_cidx_max;
      } else {
        cidx_max=tsv1_cidx_max;
      }
      
      // count diffs on the line
      int line_diff_cnt=0;
      double d1,d2;
      for (int cidx=0;cidx<cidx_max;cidx++) {
        tsv1.get(tsv1_clvl,cidx,val1);
        tsv2.get(tsv2_clvl,cidx,val2);
        // the same?
        if (val1==val2) {
          // record the base value
          if (m_residuals_tsv!=NULL) {
            // numeric?
            if (tsv1.get(tsv1_clvl,cidx,d1)==affx::TSV_OK) {
              // put a zero
              m_residuals_tsv->set(tsv1_clvl,cidx,"0");
            } else {
              // put the value
              m_residuals_tsv->set(tsv1_clvl,cidx,val1);
            }
          }
        }
        // Not string equal -- check for numeric diff
        else if ((tsv1.get(tsv1_clvl,cidx,d1)==affx::TSV_OK) &&
                 (tsv2.get(tsv2_clvl,cidx,d2)==affx::TSV_OK)) {
          double d_diff=d1-d2;
          // output the result...
          if (m_residuals_tsv!=NULL) {
            m_residuals_tsv->set(tsv1_clvl,cidx,d_diff);
          }
          // if small, it isnt a diff
          if (!((m_opt_max_diff>0.0) && (fabs(d_diff)<=m_opt_max_diff))) {
            // too big -- mark it down as a diff.
            m_diff_cnt++;
            line_diff_cnt++;
            //
          }
        }
        // normal text diff
        else {
          if (m_residuals_tsv!=NULL) {
            m_residuals_tsv->set(tsv1_clvl,cidx,"'"+val1+"'/'"+val2+"'");
          }
        }
      }
      
      // output the result line
      if (m_residuals_tsv!=NULL) {
        m_residuals_tsv->writeLevel(tsv1_clvl);
      }

      // print unchanged lines?
      if ((m_opt_print_same || (line_diff_cnt!=0)) &&
          (p_inc())) {
        //
        if (line_diff_cnt==0) {
          p_linenums(tsv1,tsv2);
          printf(" ");
          p_line(tsv1);
        }
        else if (m_opt_print_format==TsvFileDiff::FMT_1) {
          p_diff_1line(tsv1,tsv2);
        } 
        else if (m_opt_print_format==TsvFileDiff::FMT_2) {
          p_diff_2line(tsv1,tsv2);
        }
        else {
          Err::errAbort("Bad format");
        }
      }
    }
    
    //
    tsv1.nextLine();
    tsv2.nextLine();
  }
  
  close_residuals();
  
  return m_diff_cnt-start_diff_cnt;
}

// 
int
affx::TsvFileDiff::diff(affx::TsvFile& tsv1, affx::TsvFile& tsv2)
{
  int start_diff_cnt=m_diff_cnt;
  //
  if (m_opt_do_headers) {
    diffHeaders(tsv1,tsv2);
  }
  if (m_opt_do_data) {
    diffData(tsv1,tsv2);
  }
  //
  return m_diff_cnt-start_diff_cnt;
}

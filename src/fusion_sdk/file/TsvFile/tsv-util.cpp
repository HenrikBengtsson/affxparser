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



//
#include "TsvFile.h"

//
using namespace std;
using namespace affx;

//////////

void
tsv_util_usage()
{
  printf("tsv-util: option  filenames...\n"
         "    option is one of:\n"
         "      -linecount        prints the number of data lines in the file.\n"
         "      -headers          prints the headers in sorted by keys.\n"
         "\n"
         "\n");
}

void
tsv_util_linecount(char* fname)
{
  int rv;
  int data_lines;
  affx::TsvFile* tsv;

  tsv=new affx::TsvFile();
  rv=tsv->open(fname);

  printf("%-40s: ",fname);
  if (rv!=TSV_OK) {
    printf("ERR=%d\n",rv);
    return;
  }

  std::string str;
  data_lines=0;
  while (tsv->nextLine()==TSV_OK) {
    data_lines++;
    //tsv->get(0,0,str);
    //printf("%s\n",str.c_str());
  }
  printf("datalines=%d\n",data_lines);
  tsv->close();
}

void
tsv_util_headers(char* fname)
{
  int rv;
  affx::TsvFile* tsv;
  std::string key,val;

  tsv=new affx::TsvFile();
  rv=tsv->open(fname);

  if (rv!=TSV_OK) {
    printf("%s: ERR=%d\n",fname,rv);
    return;
  }

  tsv->headersBegin();
  while (tsv->headersNext(key,val)==TSV_OK) {
    printf("%s=%s\n",key.c_str(),val.c_str());
  }
  tsv->close();
}

//////////

void
tsv_util_copy(char* f_file,char* t_file,int fmt)
{
  affx::TsvFile* f_tsv=new affx::TsvFile();
  affx::TsvFile* t_tsv=new affx::TsvFile();

  //
  f_tsv->open(f_file);
  //
  t_tsv->copyFormat(*f_tsv); // better name?
  t_tsv->addHeadersFrom(*f_tsv);

  //
  if (fmt==1) {
    t_tsv->writeCsv(t_file);
  } else if (fmt==2) {
    t_tsv->writeTsv(t_file);
  } else {
    assert(0);
  }

  while (f_tsv->nextLine()==TSV_OK) {
    t_tsv->copyLevel(*f_tsv,f_tsv->line_level());
    t_tsv->writeLevel(f_tsv->line_level());
  }

  t_tsv->close();
  f_tsv->close();
}

//////////
int
main(int argc,char* argv[])
{
  // help?
  if ((argc<=1)||
      (strcmp("-help",argv[1])==0)||
      (strcmp("-h",argv[1])==0)) {
    tsv_util_usage();
    exit(0);
  }

  if ((strcmp("-linecount",argv[1])==0)||
      (strcmp("-l",argv[1])==0)) {
    int a=2;
    while (argv[a]!=NULL) {
      tsv_util_linecount(argv[a++]);
    }
    exit(0);
  }

  if (strcmp("-headers",argv[1])==0) {
    int a=2;
    while (argv[a]!=NULL) {
      tsv_util_headers(argv[a++]);
    }
    exit(0);
  }

  // Conversions
  if ((argc==4)&&(strcmp("-to-csv",argv[1])==0)) {
    tsv_util_copy(argv[2],argv[3],1);
    exit(0);
  }
  if ((argc==4)&&(strcmp("-to-tsv",argv[1])==0)) {
    tsv_util_copy(argv[2],argv[3],2);
    exit(0);
  }

  //
  tsv_util_usage();
  exit(0);
}

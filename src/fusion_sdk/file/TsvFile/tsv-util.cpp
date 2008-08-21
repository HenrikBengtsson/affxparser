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

/// @file   tsv-util.cpp
/// @brief  collection of utilities for working with tsv-format files.

//
#include "file/TsvFile/TsvFile.h"
#include "file/TsvFile/TsvFileDiff.h"
//
#include <string>
#include <vector>
//
#include "util/Err.h"
#include "util/PgOptions.h"

//////////

/// @brief     Count the datalines in the tsvfile fname.
/// @param     fileName     filename of the tsvFile
void
tsv_util_linecount(const std::string& fileName)
{
  int rv;
  int data_lines;
  affx::TsvFile* tsv;

  tsv=new affx::TsvFile();
  rv=tsv->open(fileName);

  printf("%-40s: ",fileName.c_str());
  if (rv!=affx::TSV_OK) {
    printf("ERR=%d\n",rv);
    return;
  }

  std::string str;
  data_lines=0;
  while (tsv->nextLine()==affx::TSV_OK) {
    data_lines++;
    //tsv->get(0,0,str);
    //printf("%s\n",str.c_str());
  }
  printf("datalines=%d\n",data_lines);
  tsv->close();
}

//////////

/// @brief     Dump the just the headers of the TsvFile to stdout.
/// @param     fileName  filename fo the tsvFile
void
tsv_util_headers(const std::string& fileName)
{
  int rv;
  affx::TsvFile* tsv;
  std::string key,val;

  tsv=new affx::TsvFile();
  rv=tsv->open(fileName);

  if (rv!=affx::TSV_OK) {
    printf("%s: ERR=%d\n",fileName.c_str(),rv);
    return;
  }

  tsv->headersBegin();
  while (tsv->headersNext(key,val)==affx::TSV_OK) {
    // tack on the "#%" so it looks the same as it does in the file.
    printf("#%%%s=%s\n",key.c_str(),val.c_str());
  }
  tsv->close();
}

//////////

/// @brief     Copy contents from one tsvfile to another.
/// @param     f_fileName   filename of the "from/source" tsvfile
/// @param     t_fileName   filename of the "to/destination" tsvfile
/// @param     fmt       
void
tsv_util_copy(const std::string& f_fileName,const std::string& t_fileName,int fmt)
{
  affx::TsvFile* f_tsv=new affx::TsvFile();
  affx::TsvFile* t_tsv=new affx::TsvFile();

  //
  f_tsv->open(f_fileName);
  //
  t_tsv->copyFormat(*f_tsv); // better name?
  t_tsv->addHeadersFrom(*f_tsv,affx::TSV_ADD_ALL);

  //
  if (fmt==1) {
    t_tsv->writeCsv(t_fileName);
  } else if (fmt==2) {
    t_tsv->writeTsv(t_fileName);
  } else {
    assert(0);
  }

  while (f_tsv->nextLine()==affx::TSV_OK) {
    t_tsv->copyLevel(*f_tsv,f_tsv->line_level());
    t_tsv->writeLevel(f_tsv->line_level());
  }

  t_tsv->close();
  f_tsv->close();
}

//////////

/// @brief     Paste the columns of several tsvFiles into one tsvfile
/// @param     out_fileName       filename of the output tsvfile.
/// @param     in_tsv_fileNames   vector of input tsvfile names.
/// @param     seg_max_lines      If non-zero, split the output file into segments. (0=>no segments.)
/// @param     key_col_name       Key column name to match while pasting. (""=> no key column.)
void
tsv_util_paste(const std::string& out_fileName,
               const std::vector<std::string> in_tsv_fileNames,
               int seg_max_lines,
               const std::string& key_col_name)
{
  std::vector<affx::TsvFile*> in_tsv;
  std::vector<int> in_tsv_key_cidx;
  affx::TsvFile out_tsv;
  int o_cidx;
  int seg_count=0;
  int out_tsv_lines=0;

  //
  if (seg_max_lines<0) {
    Err::errAbort("seg_max_lines must be 0 (disabled) a postive number.");
  }

  // scan the inputs to...
  for (int i=0;i<in_tsv_fileNames.size();i++) {
    // ...open
    affx::TsvFile* tsvp=new affx::TsvFile();
    tsvp->open(in_tsv_fileNames[i]);
    in_tsv.push_back(tsvp);
    // ...check for key name.
    if (key_col_name=="") { // no key name
      in_tsv_key_cidx.push_back(-1);
    }
    else {
      int key_cidx=in_tsv[i]->cname2cidx(0,key_col_name);
      if (key_cidx<0) {
        Err::errAbort("missing '"+key_col_name+"' in input '"+in_tsv_fileNames[i]+"'");
      }
      in_tsv_key_cidx.push_back(key_cidx);
    }
    // ...add header info about the input files.
    char hdr_buf[100];
    sprintf(hdr_buf,"paste-input-%05d",i);
    out_tsv.addHeader(hdr_buf,in_tsv_fileNames[i]);
  }

  // paste the column defs
  o_cidx=0;;
  for (int i=0;i<in_tsv.size();i++) {
    for (int c_idx=0;c_idx<in_tsv[i]->getColumnCount(0);c_idx++) {
      // only include the first check column, skip the rest.
      if ((c_idx!=in_tsv_key_cidx[i])||(i==0)) {
        out_tsv.defineColumn(0,o_cidx++,in_tsv[i]->getColumnName(0,c_idx));
      }
    }
  }

  // open the output file in the unsegmented case
  if (seg_max_lines==0) {
    out_tsv.writeTsv_v1(out_fileName);
  }

  // paste the columns
  while (1) {
    // open a segmented output file as needed
    if ((seg_max_lines>0) && // have segments?
        ((out_tsv_lines>=seg_max_lines) || // hit seg size?
         ((seg_count==0)&&(out_tsv_lines==0)) // first time through?
         )) {
      char buf[300];
      sprintf(buf,"%s-%04d",out_fileName.c_str(),seg_count++);
      out_tsv.writeTsv_v1(buf);
      out_tsv_lines=0;
    };

    std::string key_col_value;

    bool more_lines=false;
    for (int i=0;i<in_tsv.size();i++) {
      if (in_tsv[i]->nextLevel(0)==affx::TSV_OK) {
        more_lines=true;
      }
    }
    if (!more_lines) {
      break;
    }
    // copy to out
    o_cidx=0;
    for (int i=0;i<in_tsv.size();i++) {
      for (int i_cidx=0;i_cidx<in_tsv[i]->getColumnCount(0);i_cidx++) {
        // get the input.
        std::string tmp;
        int rv=in_tsv[i]->get(0,i_cidx,tmp);
        // is this a check column?
        if (i_cidx==in_tsv_key_cidx[i]) {
          // must be set
          if (rv!=affx::TSV_OK) {
            Err::errAbort("check column value is required. '"+in_tsv_fileNames[i]+"'");
          }
          // the first is the reference
          if (i==0) {
            out_tsv.set(0,o_cidx++,tmp);
            key_col_value=tmp;
          }
          else {
            if (tmp!=key_col_value) {
              Err::errAbort("Check column mismatch.  ref='"+key_col_value+"'  val='"+tmp+"'");
            }
          }
        }
        // a normal column
        else {
          out_tsv.set(0,o_cidx++,tmp);
        }          
      }
    }
    //
    out_tsv.writeLevel(0);
    out_tsv_lines++;

  } // while

  // clean up
  out_tsv.close();
  //
  for (int i=0;i<in_tsv.size();i++) {
    in_tsv[i]->close();
    delete in_tsv[i];
  }
}

//////////

#define BENCHMARK_COL_CNT 10

void
tsv_util_benchmark(int benchmark_cnt,const std::string& filename)
{
  if (benchmark_cnt<=0) {
    printf("Need to write at least one double.");
    return;
  }
  if (filename=="") {
    printf("Need to give a file to write to.");
    return;
  }

  int rv;
  affx::TsvFile* tsv;
  
  tsv=new affx::TsvFile();

  tsv->addHeader("benchmark-cnt",benchmark_cnt);
  tsv->addHeader("benchmark-file",filename);

  for (int col=0;col<BENCHMARK_COL_CNT;col++) {
    char buf[100];
    sprintf(buf,"col-%04d",col);
    tsv->defineColumn(0,col,buf);
  }
  
  rv=tsv->writeTsv_v1(filename);

  int cnt=0;
  while (cnt<benchmark_cnt) {
    for (int c=0;c<BENCHMARK_COL_CNT;c++) {
      tsv->set(0,c,(cnt++)+0.123456);
    }
    tsv->writeLevel(0);
  }
  //
  tsv->close();
}

//////////

/// @brief     entry point for tsv-util
/// @param     argc      
/// @param     argv      
/// @return    
int
main(int argc,char* argv[])
{
  PgOptions* opts=new PgOptions();
  //
  opts->setUsage("Utility functions for tsv files."
                 "\n"
                 "\n"
                 );
  //
  opts->defineOption("h","help", PgOpt::BOOL_OPT,
                    "This message.",
                    "false");

  opts->defineOption("","headers", PgOpt::BOOL_OPT,
                    "INFILE = Display the headers of the CSV/TSV file.",
                    "false");
  opts->defineOption("","linecount", PgOpt::BOOL_OPT,
                    "INFILE = count the lines of data in the file.",
                    "false");
  opts->defineOption("","to-csv", PgOpt::BOOL_OPT,
                    "INFILE OUTFILE = Covert the file to CVS format.",
                    "false");
  opts->defineOption("","to-tsv", PgOpt::BOOL_OPT,
                    "INFILE OUTFILE = Convert the file to TSV format.",
                    "false");
  //
  opts->defineOption("","paste", PgOpt::STRING_OPT,
                    "OUTFILE  FILELIST = Paste the files of filelist into outfile.",
                    "");
  opts->defineOption("","seg-lines",PgOpt::INT_OPT,
                    " Segment size",
                    "0");
  opts->defineOption("","key-col", PgOpt::STRING_OPT,
                    "COLUMN_NAME = Column name which must be present and equal in all inputs files, if set.",
                    "");
  //
  opts->defineOption("","benchmark", PgOpt::INT_OPT,
                     "Number of doubles to write for benchmarking output.",
                     "0");
  opts->defineOption("","output", PgOpt::STRING_OPT,
                     "Output file for some operations.",
                    "");
  //
  opts->defineOption("","diff",PgOpt::BOOL_OPT,
                     "Compare two tsv formatted files. "
                     "There are a number of options to control printing of the diff.",
                     "");
  //
  opts->defineOption("","diff-headers",PgOpt::BOOL_OPT,
                     "BOOL = Compare the headers of the files.",
                     "true");
  opts->defineOption("","diff-data",PgOpt::BOOL_OPT,
                     "BOOL = Compare the data in the files.",
                     "true");
  //
  opts->defineOption("","diff-print-linenums",PgOpt::BOOL_OPT,
                     "BOOL = Print line numbers from where the lines are from (file1:file2).",
                     "true");
  opts->defineOption("","diff-print-same",PgOpt::BOOL_OPT,
                     "BOOL = Print lines which are the same. "
                     "(otherwise only changed lines.)",
                     "false");
  opts->defineOption("","diff-print-max",PgOpt::INT_OPT,
                     "NUM = Max number of lines to print. (-1=> all)",
                     "-1");
  opts->defineOption("","diff-print-format",PgOpt::INT_OPT,
                     "[1 or 2] = Select the format of the displayed diffs, one or two lines.",
                     "1");
  opts->defineOption("","diff-max-diff",PgOpt::DOUBLE_OPT,
                     "NUM = Numerical differences smaller than than this are ignored.",
                     "0.00001");
  opts->defineOption("","diff-residuals",PgOpt::STRING_OPT,
                     "FILENAME = File to output residuals to. (file1-file2)",
                     "");

  //
  opts->parseArgv(argv);
  
  //
  if (opts->getBool("help")||(argc==1)) {
    opts->usage();
    exit(0);
  }

  else if (opts->getBool("linecount")) {
    for (int a=0;a<opts->getArgCount();a++) {
      tsv_util_linecount(opts->getArg(a));
    }
  }
  else if (opts->getBool("headers")) {
    for (int a=0;a<opts->getArgCount();a++) {
      tsv_util_headers(opts->getArg(a));
    }
  }

  // Conversions
  else if (opts->getBool("to-csv")) {
    if (opts->getArgCount()<2) {
      printf("Need two args.");
      opts->usage();
    }
    tsv_util_copy(opts->getArg(0),opts->getArg(1),1);
  }
  else if (opts->getBool("to-tsv")) {
    if (opts->getArgCount()<2) {
      printf("Need two args.");
      opts->usage();
    }
    tsv_util_copy(opts->getArg(0),opts->getArg(1),2);
  }
  
  //
  else if (opts->get("paste")!="") {
    tsv_util_paste(opts->get("paste"),
                   opts->getArgVector(),
                   opts->getInt("seg-lines"),
                   opts->get("key-col"));
  }
  //
  else if (opts->getBool("diff")) {
    if (opts->getArgCount()!=2) {
      printf("Need exactly two args.");
      opts->usage();
    }
    else {
      affx::TsvFileDiff diff;
      // set our options
      diff.m_opt_do_headers=opts->getBool("diff-headers");
      diff.m_opt_do_data=opts->getBool("diff-data");
      //
      diff.m_opt_print_linenums=opts->getBool("diff-print-linenums");
      diff.m_opt_print_same=opts->getBool("diff-print-same");
      diff.m_opt_print_format=opts->getInt("diff-print-format");
      diff.m_opt_print_max=opts->getInt("diff-print-max");
      diff.m_opt_max_diff=opts->getDouble("diff-max-diff");
      diff.m_residuals_filename=opts->get("diff-residuals");
      //diff.m_residuals_filename=opts->getArg(1)+".residuals";
      // do the diff
      diff.diff(opts->getArg(0),opts->getArg(1));
      //
      if (diff.m_diff_cnt!=0) {
        printf("### %d diff\n",diff.m_diff_cnt);
      }
    }
  }
  //
  else if (opts->getInt("benchmark")!=0) {
    tsv_util_benchmark(opts->getInt("benchmark"),opts->get("output"));
  }

  //
  delete opts;
}

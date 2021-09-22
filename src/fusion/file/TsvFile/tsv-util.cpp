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
#include "util/Convert.h"
#include "util/Err.h"
#include "util/Fs.h"
#include "util/PgOptions.h"
#include "util/Util.h"
//
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
//

#ifdef WIN32
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif

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
    delete tsv;
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
  delete tsv;
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
    delete tsv;
    return;
  }

  tsv->headersBegin();
  while (tsv->headersNext(key,val)==affx::TSV_OK) {
    // tack on the "#%" so it looks the same as it does in the file.
    printf("#%%%s=%s\n",key.c_str(),val.c_str());
  }
  tsv->close();
  delete tsv;
}

//////////

int
tsv_util_print_duplicate_headerkeys(const std::string& fileName)
{
  int rv;
  affx::TsvFile* tsv;

  tsv=new affx::TsvFile();
  rv=tsv->open(fileName);

  if (rv!=affx::TSV_OK) {
    printf("%s: ERR=%d\n",fileName.c_str(),rv);
    delete tsv;
    return 1;
  }

  int cnt=tsv->printDuplicateHeaders();

  tsv->close();
  delete tsv;

  return (cnt==0)?0:1;
}

//////////

/// @brief     Copy contents from one tsvfile to another.
/// @param     f_fileName   filename of the "from/source" tsvfile
/// @param     t_fileName   filename of the "to/destination" tsvfile
/// @param     fmt
void
tsv_util_copy(const std::string& f_fileName,const std::string& t_fileName,int fmt)
{
  affx::TsvFile f_tsv;
  affx::TsvFile t_tsv;

  //
  f_tsv.open(f_fileName);
  //
  t_tsv.copyFormat(f_tsv); // better name?
  t_tsv.addHeadersFrom(f_tsv,affx::TSV_ADD_ALL);

  //
  if (fmt==1) {
    t_tsv.writeCsv(t_fileName);
  } else if (fmt==2) {
    t_tsv.writeTsv(t_fileName);
  } else {
    assert(0);
  }

  while (f_tsv.nextLine()==affx::TSV_OK) {
    t_tsv.copyLevel(f_tsv,f_tsv.lineLevel());
    t_tsv.writeLevel(f_tsv.lineLevel());
  }

  t_tsv.close();
  f_tsv.close();
}

//////////

/// The limit of filehandles we can paste at one time.
// #define TSV_PASTE_FH_LIMIT 900
// for testing
#define TSV_PASTE_FH_LIMIT 900

/// @brief     Paste the columns of several tsvFiles into one tsvfile in one pass.
///            Cant to more than the limit in one pass.
/// @param     out_fileName       filename of the output tsvfile.
/// @param     in_tsv_fileNames   vector of input tsvfile names.
/// @param     in_header_fileNames   vector of names to put in header.
///                                  (This is needed for the final multi-pass case.)
/// @param     seg_max_lines      If non-zero, split the output file into segments. (0=>no segments.)
/// @param     key_col_name       Key column name to match while pasting. (""=> no key column.)
/// @param     verbose            The verbosity level
void
tsv_util_paste_basic(const std::string& out_fileName,
                     const std::vector<std::string> in_tsv_fileNames,
                     const std::vector<std::string> in_header_fileNames,
                     int seg_max_lines,
                     const std::string& key_col_name,
                     int verbose)
{
  std::vector<affx::TsvFile*> in_tsv;
  std::vector<int> in_tsv_key_cidx;
  affx::TsvFile out_tsv;
  int o_cidx;
  int seg_count=0;
  int out_tsv_lines=0;

  //
  if (in_tsv_fileNames.size()>TSV_PASTE_FH_LIMIT) {
    // @todo: why doesnt '#TSV_PASTE_FH_LIMIT' work here?
    Err::errAbort("tsv-paste: Can only paste " +ToStr(TSV_PASTE_FH_LIMIT)+
                  " tsv files at at time. (unix fh limit.)");
  }

  //
  if (seg_max_lines<0) {
    Err::errAbort("tsv-paste: seg_max_lines must be 0 (disabled) a postive number.");
  }

  // some output for the user
  if (verbose>=2) {
    for (int i=0;i<in_tsv_fileNames.size();i++) {
      printf("# %3d : %s\n",i,in_tsv_fileNames[i].c_str());
    }
    printf("# === > %s\n",out_fileName.c_str());
  }

  // the key_col_name might be numeric.
  char* key_col_name_ptr2;
  const char* key_col_name_ptr1=key_col_name.c_str();
  int key_col_num=strtol(key_col_name_ptr1,&key_col_name_ptr2,10);
  // not a full conversion.
  if (*key_col_name_ptr2!=0) {
    key_col_num=-1;
  }

  // scan the inputs to...
  for (int i=0;i<in_tsv_fileNames.size();i++) {
    // ...open
    affx::TsvFile* tsvp=new affx::TsvFile();
    tsvp->open(in_tsv_fileNames[i]);
    in_tsv.push_back(tsvp);
    //
    if (verbose>=3) {
      printf("tsv_paste: open '%s'\n",in_tsv_fileNames[i].c_str());
    }
    // ...check for key name.
    if (key_col_name=="") { // no key name
      in_tsv_key_cidx.push_back(-1);
    }
    else {
      int key_cidx;
      key_cidx=in_tsv[i]->cname2cidx(0,key_col_name);
      if (key_col_num>=0) {
        key_cidx=key_col_num;
      }
      else {
        Err::errAbort("tsv-paste: missing column '"+key_col_name+"' in input '"+in_tsv_fileNames[i]+"'");
      }
      if ((key_cidx<0)||(key_cidx>=in_tsv[i]->getColumnCount(0))) {
        Err::errAbort("tsv-paste: key column is out of bounds. (key_cidx="+ToStr(key_cidx)+")");
      }
      in_tsv_key_cidx.push_back(key_cidx);
    }
  }

  // ...add header info describing the input files used.
  for (int i=0;i<in_header_fileNames.size();i++) {
    char hdr_buf[100];
    SNPRINTF(hdr_buf,sizeof(hdr_buf),"paste-input-%05d",i);
    out_tsv.addHeader(hdr_buf,in_header_fileNames[i]);
  }

  // paste the column defs into the output tsv
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

  // paste the column while we have rows.
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

    // copy row data to the output tsv.
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
            Err::errAbort("tsv-paste: Check column value is required. '"+in_tsv_fileNames[i]+"'");
          }
          // the first is the reference
          if (i==0) {
            out_tsv.set(0,o_cidx++,tmp);
            key_col_value=tmp;
          }
          else {
            if (tmp!=key_col_value) {
              Err::errAbort("tsv-paste: Check column mismatch.  "
                            "file='"+in_tsv_fileNames[i]+"' "
                            "line="+ToStr(in_tsv[i]->lineNum())+" "
                            "ref='"+key_col_value+"' "
                            "val='"+tmp+"'");
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

/// @brief     Paste the columns of the input tsv files into a big tsv file.
///            If there are too many inputs, do the work in several passes.
///            Also splits the output into seg_max_line size files if requested.
/// @param     out_fileName
/// @param     in_tsv_fileNames
/// @param     seg_max_lines
/// @param     key_col_name
/// @param     verbose
/// @param     tsv_paste_fh_limit
void
tsv_util_paste(const std::string& out_fileName,
               const std::vector<std::string> in_tsv_fileNames,
               int seg_max_lines,
               const std::string& key_col_name,
               int verbose,
               int tsv_paste_fh_limit)
{
  // the simple case, no passes needed...
  if (in_tsv_fileNames.size()<=tsv_paste_fh_limit) {
    // ...so just do it.
    tsv_util_paste_basic(out_fileName,
                         in_tsv_fileNames,in_tsv_fileNames,
                         seg_max_lines,key_col_name,verbose);
    // and we are done.
    return;
  }

  // complex case, passes are needed.
  if (verbose>=1) {
    printf("tsv_util_paste: multiple passes needed to process '%d' inputs.\n",
           int(in_tsv_fileNames.size()));
  }

  // we have to be able to open at least two files at a time.
  if (tsv_paste_fh_limit<=1) {
    Err::errAbort("tsv_paste_fh_limit must be bigger than 2.");
  }

  //
  std::vector<std::string> copy_fileNames=in_tsv_fileNames;
  //
  int pass_cnt=0;
  std::string pass0_out_name;
  std::string pass1_out_name;
  std::vector<std::string> pass_out_names;
  std::vector<std::string> pass1_fileNames;

  // while we have more than the limit, we need to use a temp file.
  while (copy_fileNames.size()>tsv_paste_fh_limit) {
    pass1_fileNames.clear();
    // tack on the prior tmp file.
    if (pass0_out_name!="") {
      pass1_fileNames.push_back(pass0_out_name);
    }
    // add some more input files.
    // dont adjust the length for the file we pushed on.
    int len=tsv_paste_fh_limit;
    pass1_fileNames.insert(pass1_fileNames.end(),
                           copy_fileNames.begin(),copy_fileNames.begin()+len);
    copy_fileNames.erase(copy_fileNames.begin(),copy_fileNames.begin()+len);
    // gen the name for the new temp file.
    char buf[100];
    SNPRINTF(buf,sizeof(buf),"-%05d.tmp",pass_cnt);
    pass1_out_name=out_fileName+buf;
    pass_out_names.push_back(pass1_out_name);
    //
    if (verbose>=1) {
      printf("pass: %d : '%s'...\n",pass_cnt,pass1_out_name.c_str());
    }
    // paste what we have into the new tmp file.
    tsv_util_paste_basic(pass1_out_name,
                         pass1_fileNames,
                         pass1_fileNames, // the filenames in this pass
                         0, // dont segment.
                         key_col_name,verbose);
    // get rid of the old tmp file.
    if (pass0_out_name!="") {
      //printf("rm %s\n",pass0_out_name.c_str());
      Fs::rm(pass0_out_name);
    }
    // the current is now the old.
    pass_cnt++;
    pass0_out_name=pass1_out_name;
  }
  // now do the last pass into the official output file.
  pass1_fileNames.clear();
  if (pass0_out_name!="") {
    pass1_fileNames.push_back(pass0_out_name);
  }
  pass1_fileNames.insert(pass1_fileNames.end(),copy_fileNames.begin(),copy_fileNames.end());
  tsv_util_paste_basic(out_fileName, // the final output file
                       pass1_fileNames,
                       in_tsv_fileNames, // the filenames for all the inputs.
                       seg_max_lines, // do segment if needed.
                       key_col_name,verbose);
  // and get rid of it.
  Fs::rm(pass0_out_name);
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
  delete tsv;
}

//////////

/// @brief     entry point for tsv-util
/// @param     argc
/// @param     argv
/// @return
int
main(int argc,const char* argv[])
{
  int rv=0;
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
  opts->defineOption("v","verbose",PgOpt::INT_OPT,
                     "verbose level",
                     "0");

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
                     "Segment size",
                     "0");
  opts->defineOption("","key-col", PgOpt::STRING_OPT,
                     "COLUMN_NAME = Column name which must be present and equal in all inputs files, if set.",
                     "");
  opts->defineOption("","max-paste-fh", PgOpt::INT_OPT,
                     "CNT = Max number of files to paste in one pass.",
                     "900");
  //
  opts->defineOption("","benchmark", PgOpt::INT_OPT,
                     "Number of doubles to write for benchmarking output.",
                     "0");
  opts->defineOption("","output", PgOpt::STRING_OPT,
                     "Output file for some operations.",
                     "");
  //
  opts->defineOption("","diff",PgOpt::BOOL_OPT,
                     "Compare headers and data of two tsv formatted files. "
                     "There are a number of options to control printing of the diff.",
                     "false");
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

  opts->defineOption("","print-duplicate-headerkeys",PgOpt::BOOL_OPT,
                     "Print the duplicate headers.",
                     "0");
  
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
                   opts->get("key-col"),
                   opts->getInt("verbose"),
                   opts->getInt("max-paste-fh"));
  }
  //
  else if (opts->getBool("diff")||opts->getBool("diff-headers")||opts->getBool("diff-data")) {
    if (opts->getArgCount()!=2) {
      printf("Need exactly two args.");
      opts->usage();
    }
    else {
      // --diff implies both headers and data
      if (opts->getBool("diff")) {
        if (!opts->mustFindOpt("diff-headers")->isSet()) {
          opts->mustFindOpt("diff-headers")->setValue("1");
        }
        if (!opts->mustFindOpt("diff-data")->isSet()) {
          opts->mustFindOpt("diff-data")->setValue("1");
        }
      }
      //
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
  else if (opts->getBool("print-duplicate-headerkeys")) {
    int cnt=0;
    for (int i=0;i<opts->getArgCount();i++) {
      if (tsv_util_print_duplicate_headerkeys(opts->getArg(i))) {
        cnt++;
      }
    }
    // dont fiddle with the exit value.
    // rv=(cnt==0)?0:1;
  }
  //
  else if (opts->getInt("benchmark")!=0) {
    tsv_util_benchmark(opts->getInt("benchmark"),opts->get("output"));
  }

  //
  delete opts;
  return rv;
}

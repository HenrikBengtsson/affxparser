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
 * \file tsv-example.cpp
 * \brief This program is provided as an example of using TsvFile.
 *        You should be able to find a common uses of TsvFile here
 *        to use as a starting point in your programs.
 *        Read \link file-format-tsv the TsvFile docs \endlink more about TsvFile.
 */

//
#include "file/TsvFile/ClfFile.h"
#include "file/TsvFile/PgfFile.h"
#include "file/TsvFile/TsvFile.h"
//
#include <cstring>
#include <string.h>
#include <string>
//


//
//
#ifdef _MSC_VER
#define snprintf _snprintf
#endif

//
using namespace std;
using namespace affx;

/// When set, dont generate much output -- used when benchmarking the code
int opt_benchmark;
/// How much output to generate
int opt_verbose;

///// Simple loop example (on a clf file)

/// @brief     Walk the contents of a clf file and count the entries
/// @param     file_name
void
example_walk_clf(std::string file_name)
{
  printf("=== example walk clf ====================\n");

  int rv;
  affx::ClfFile clf;

  clf.m_tsv.m_optAbortOnError=false; // this should be true in real code.

  if ((rv=clf.open(file_name))!=TSV_OK) {
    printf("open of '%s' failed! (rv=%d)\n",file_name.c_str(),rv);
    return;
  }

  int cnt_probe=0;
  while (clf.next_probe()==TSV_OK) {
    cnt_probe++;
  }
  printf("'%s' has %d probes\n",file_name.c_str(),cnt_probe);
}

///// Nested loop example (on a pgf file)

/// @brief     Walk the contents of a PGF file and count the number of each items.
/// @param     file_name
void
example_walk_pgf(std::string file_name)
{
  printf("=== example walk pgf ====================\n");

  int rv;
  affx::PgfFile* pgf;

  pgf=new affx::PgfFile();
  pgf->m_tsv.m_optAbortOnError=false; // this should be true in real code.
  if ((rv=pgf->open(file_name))!=TSV_OK) {
    printf("open of '%s' failed! (rv=%d)\n",file_name.c_str(),rv);
    delete pgf;
    return;
  }

  int cnt_probeset=0;
  int cnt_atoms=0;
  int cnt_probes=0;
  while (pgf->next_probeset()==TSV_OK) {
    cnt_probeset++;
    while (pgf->next_atom()==TSV_OK) {
      cnt_atoms++;
      while (pgf->next_probe()==TSV_OK) {
        cnt_probes++;
      }
    }
  }
  printf("read '%s' with %d probesets, %d atoms, %d probes.\n",
         file_name.c_str(),cnt_probeset,cnt_atoms,cnt_probes);

  pgf->close();
  delete pgf;
}

/////

/// @brief     Query the tsv file by gc_count and print out the matching probes.
/// @param     file_name Filename to search
void
example_index_1(std::string file_name)
{
  printf("=== example index 1 ====================\n");

  int rv;
  affx::TsvFile* tsv;

  int match_cnt=0;
  int match_print_max=20; // dont print more than this.
  std::string match_seq;
  int match_gc=0;
  int query_gc;
  const char* opstr;
  opstr="??";

  //
  tsv=new affx::TsvFile();
  if ((rv=tsv->open(file_name))!=TSV_OK) {
    printf("open of '%s' failed! (rv=%d)\n",file_name.c_str(),rv);
    delete tsv;
    return;
  }

  // we need an index
  tsv->defineIndex(2,"gc_count",TSV_INDEX_INT,0);

  // start the search
  tsv->findBegin(2,"gc_count",TSV_OP_LTEQ,query_gc=7,TSV_ORDERBY_VAL);
  opstr="<=";
  // tsv->findBegin(2,"gc_count",TSV_OP_GTEQ,query_gc=18,TSV_ORDERBY_VAL); opstr=">=";
  // tsv->findBegin(2,"gc_count",TSV_OP_EQ,query_gc=11,TSV_ORDERBY_VAL); opstr="==";
  // tsv->dump_indexes();

  //
  printf("searching for probes (gc_cnt %s %d) ==========\n",opstr,query_gc);
  printf("hit | line    | seq                       | gc \n");
  printf("----+---------+---------------------------+----\n");
  while (tsv->findNext()==TSV_OK) {
    match_cnt++;
    tsv->get(2,"probe_sequence",match_seq);
    tsv->get(2,"gc_count",match_gc);
    
    if (opt_verbose>0) {
      if (match_cnt<match_print_max) {
        printf("%3d | %7u | %-25s | %4d\n",
               match_cnt,tsv->lineNumber(),match_seq.c_str(),match_gc);
      }
      else if (match_cnt==match_print_max) {
        printf("first %d matches printed; skipping remaining matches...\n",match_print_max);
      }
    }
  }
  printf("=== %6d probes with gc_count %s %4d\n",match_cnt,opstr,query_gc);
  //
  tsv->close();
  delete tsv;
}


/// @brief     Search a pgf file for a matching sequence
/// @param     file_name
void
example_index_2(std::string file_name)
{
  printf("=== example index 2 ====================\n");

  affx::PgfFile pgf;
  std::string query_seq="GTTTCTTATACGCTTACTTCGACAA";

  printf("searching for probes matching '%s'...\n",query_seq.c_str());

  pgf.open(file_name);
  pgf.m_tsv.defineIndex(2,"probe_sequence",TSV_INDEX_STRING,0);
  pgf.m_tsv.findBegin(2,"probe_sequence",TSV_OP_EQ,query_seq);
  //
  printf("result count: %d \n",pgf.m_tsv.findResultsCount());
  while (pgf.m_tsv.findNext()==TSV_OK) {
    if (opt_verbose>0) {
      printf("%5d : %10d : %-30s\n",pgf.m_tsv.lineNumber(),pgf.probe_id,pgf.probe_sequence.c_str());
    }
  }
}

/// @brief     Example of writing a custom TSV file.
/// @param     file_name file to write to
void
example_write_1(std::string  file_name)
{
  affx::TsvFile tsv;

  // two keys with different values.
  // Add all your keys before calling "writeTsv".
  tsv.addHeader("Example","This an example header. (1)");
  tsv.addHeader("Example","This an example header. (2)");
  // why? because "headerN" is a reserved key.
  tsv.addHeader("header0","this should not appear in the output");
  // "header_whatever" isnt
  tsv.addHeader("header_test","this should appear in the output");
  // The "%6.2" might be used in the future.
  // for now the format info is ignored.
  tsv.defineFile("col00\tcol01\tcol02\n"
                 "col10\tcol11\tcol12\tcol13\n"
                 "col20,string\tcol21,integer\tcol22,float,%6.2f\tcol23\tcol24\n");

  tsv.writeTsv(file_name);
  // keys added after here are still added, but wont appear in the output...
  // ...the header has been written already.

  // Stuff data into the levels and columns and write it out.
  int cnt=0;
  int r0_reps=3; // count of topmost lines
  int rN_reps=6; // count of inner most lines
  for (int r0=0;r0<r0_reps;r0++) {
    for (int clvl=0;clvl<tsv.getLevelCount();clvl++) {
      for (int rN=0;rN<((clvl<(tsv.getLevelCount()-1)?1:rN_reps));rN++) {
        for (int cidx=0;cidx<tsv.getColumnCount(clvl);cidx++) {
          tsv.set(clvl,cidx,cnt++);
        }
        tsv.writeLevel(clvl);
      }
    }
  }

  tsv.close();
}

/////

/// @brief     Example of writing the start of a PgfFile
/// @param     file_name where to write.
void
example_write_2(std::string file_name)
{
  affx::PgfFile pgf;

  pgf.defineFilePgf();
  pgf.m_tsv.addHeader("Note","Add headers before writing!");
  pgf.m_tsv.addHeader("Note","Adding headers later doesnt put them in the file.");
  pgf.m_tsv.addHeader("Example-02","Feel free to delete it.");
  pgf.m_tsv.addHeader("Example-01","This is an example file.");
  pgf.write(file_name);
  // write one probe
  pgf.m_tsv.set(0,"probeset_id"  ,1354897);
  pgf.m_tsv.set(0,"probeset_name","204339_s_at");
  pgf.m_tsv.writeLevel(0);
  //
  pgf.m_tsv.set(1,"atom_id",1354898);
  pgf.m_tsv.writeLevel(1);
	//
  pgf.m_tsv.set(2,"probe_id"              ,1221821);
  pgf.m_tsv.set(2,"type"                  ,"pm:target->at");
  pgf.m_tsv.set(2,"gc_count"              ,13);
  pgf.m_tsv.set(2,"probe_length"          ,25);
  pgf.m_tsv.set(2,"interrogation_position",13);
  pgf.m_tsv.set(2,"probe_sequence"        ,"ACAACGACCGTTCCGGAATCGACAT");
  pgf.m_tsv.set(2,"exon_position"         ,1703);
  pgf.m_tsv.writeLevel(2);
  //
  pgf.m_tsv.close();
}

/////

/// @brief     write v1 tsv and csv files
/// @param     file_name name of file 
/// @param     fmt       1 for tsv, 2 for csv
void
example_write_3(std::string file_name,int fmt)
{
  affx::TsvFile tsv;

  tsv.defineFile("col00\tcol01\tcol02\tcol03");
  tsv.addHeader("Note","This is example output");

  //tsv.dump_headers();

  if (fmt==1) {
    tsv.writeTsv_v1(file_name);
  } else if (fmt==2) {
    tsv.writeCsv(file_name);
  } else {
    assert(0);
  }

  int cnt=0;
  for (int i=0;i<100;i++) {
    tsv.set(0,"col00",cnt++);
    tsv.set(0,"col01",cnt++);
    tsv.set(0,"col02",cnt++);
    tsv.set(0,"col03",cnt++);
    tsv.writeLevel(0);
  }
  tsv.close();
}

/// @brief     Example of writing a custom TSV file.
/// @param     file_name file to write to
/// @param     row_cnt   number of rows to write
/// @param     col_cnt   number of colss to write
void
example_write_4(std::string file_name,int row_cnt,int col_cnt)
{
  affx::TsvFile tsv;
  char string_buf[100];

  snprintf(string_buf,sizeof(string_buf),
           "'%s' %d rows, %d cols",
           file_name.c_str(),row_cnt,col_cnt);
  tsv.addHeader("Note",string_buf);

  for (int c=0;c<col_cnt;c++) {
    snprintf(string_buf,sizeof(string_buf),"col%03d",c);
    tsv.defineColumn(0,c,string_buf);
    tsv.setPrecision(0,c,c+1);
  }

  tsv.writeTsv(file_name);

  int i=0;
  for (int r=0;r<row_cnt;r++) {
    for (int c=0;c<col_cnt;c++) {
      tsv.set(0,c,(i++)+0.1234);
    }
    tsv.writeLevel(0);
  }
  tsv.close();
}

/////

/// @brief     An example of how to read a table
/// @param     file_name table file to be read
void
example_read_table_1(std::string file_name)
{
  affx::TsvFile tsv;

  printf("=== example table read ====================\n");

  tsv.openTable(file_name);

  while (tsv.nextLevel(0)==TSV_OK) {
    int cidx=0;
    string colstr;
    while ((cidx<tsv.getColumnCount(tsv.lineLevel())) &&
           (tsv.get(tsv.lineLevel(),cidx,colstr)==TSV_OK)) {
      if (cidx>0) {
        printf("\t");
      }
      printf("%s",colstr.c_str());
      cidx++;
    }
    printf("\n");
  }
  tsv.close();
}

/// @brief     example of dumping columns
/// @param     file_name file to be read and dumped
void
example_dump_columns(std::string file_name)
{
  affx::TsvFile tsv;
  int rv;

  printf("=== example column dump: %s ====================\n",file_name.c_str());
  
  rv=tsv.openTable(file_name);
  assert(rv==TSV_OK);

  int clvl_max=tsv.getLevelCount();
  for (int clvl=0;clvl<clvl_max;clvl++) {
    int cidx_max=tsv.getColumnCount(clvl);
    for (int cidx=0;cidx<cidx_max;cidx++) {
      std::string cname;
      rv=tsv.cidx2cname(clvl,cidx,cname);
      assert(rv==TSV_OK);
      printf("%3d : %3d : %s\n",clvl,cidx,cname.c_str());
    }
  }
  tsv.close();

  printf("\n");
}

//////////

/// @brief     Print usage instructions
void
usage()
{
  printf(
         "\n"
         "usage: tsv-example [-b] testfilename.pgf\n"
         "\n"
         "   This is an example of common usage of TsvFile.\n"
         "   It checks that the file is of the correct type and\n"
         "   scans through it, indexes it and does some searching.\n"
         "\n"
         "   The '-b' flag suppresses some of the output so the time reflect\n"
         "   running time not printing time.\n"
         "\n"
         );
}

/// @brief     entry point for the program
/// @param     argc   argument count
/// @param     argv   vector of arguments
/// @return    exit value
int
main(int argc,char* argv[])
{
  char** arg_next;

  //
  opt_benchmark=0;
  opt_verbose=1;

  //
  arg_next=argv;
  arg_next++;
  if ((*arg_next!=NULL)&&(strcmp(*arg_next,"-h")==0)) {
    usage();
    exit(0);
  }
  if ((*arg_next!=NULL)&&(strcmp(*arg_next,"-b")==0)) {
    opt_benchmark=1;
    opt_verbose=0;
    printf("=== benchmarking mode...\n");
    arg_next++;
  }
  if (*arg_next==NULL) {
    usage();
    exit(0);
  }

  // people dont like bad format messages
  // example_walk_clf(*arg_next);

  // walk a pgf

  example_walk_pgf(*arg_next);

  // a couple times for profiling
  if (0) {
    for (int i=0;i<2;i++) {
      printf("=== %3d: ",i);
      example_walk_pgf(*arg_next);
    }
  }

  //
  example_index_1(*arg_next);
  example_index_2(*arg_next);
  //
  example_write_1("example-write-1.tsv");
  example_write_2("example-write-2.pgf");
  //
  example_write_3("example-write-3.tsv",1);
  example_write_3("example-write-3.csv",2);

  // commented out as this writes a big file.
  // example_write_4("/var/tmp/example-write-4.tsv",1000000,5);

  //
  example_read_table_1("data-table-1.txt");
  //
  example_dump_columns(*arg_next);

  //
  printf("=== done ===\n");

  return 0;
}

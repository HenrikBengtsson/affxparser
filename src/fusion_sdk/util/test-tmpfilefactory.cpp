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
#include "util/Fs.h"
#include "util/PgOptions.h"
#include "util/TmpFileFactory.h"
#include "util/Util.h"
//
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
//

//////////

int opt_debug=0;
int opt_verbose=0;

//////////

/// @brief     Dump the filename to the file as test data.
/// @param     filename  filename to create
void
test_write_data(const std::string& filename)
{
  assert(!Fs::isReadable(filename));

  std::ofstream outs;
  outs.open(filename.c_str());
  outs<<filename<<std::endl;
  outs.close();
}

//////////

void
test_1()
{
  // make a new factory to generate temp files.
  TmpFileFactory* tff=new TmpFileFactory();
  std::string tfn;
  tff->setPrefix("tff-test1-");
  tff->setDebug(opt_debug);
  tff->setVerbose(opt_verbose);
  //
  for (int i=0;i<10;i++) {
    // this allocates a tmp file name and remembers it.
    tfn=tff->genFilename();
    if (opt_verbose>0) {
      printf("%4d : '%s'\n",i,tfn.c_str());
    }
    // put something in the file.
    test_write_data(tfn);
  }
  // When the object is deleted, it cleans up all the tmpfiles.
  delete tff;
}

/// this function has a TmpFileFactory pointer passed into it.
/// the allocations are done within its context.
void
test_2_helper(TmpFileFactory* tff,int cnt)
{
  for (int i=0;i<cnt;i++) {
    printf("%4d : '%s'\n",i,tff->genFilename().c_str());
  }
}

void
test_2()
{
  TmpFileFactory* tff=new TmpFileFactory();
  tff->setPrefix("tff-test2-");
  tff->setDebug(opt_debug);
  tff->setVerbose(opt_verbose);
  // passing the context into a fuctions
  test_2_helper(tff,5);
  test_2_helper(tff,5);
  // now clean up.
  delete tff;
}

void
test_3()
{
  TmpFileFactory* tff=new TmpFileFactory();
  tff->setPrefix("tff-test3-");
  tff->setDebug(opt_debug);
  tff->setVerbose(opt_verbose);
  //
  for (int i=0;i<10;i++) {
    tff->genDirname();
  }
  // make a subdir and allocate future tmp files in that subdir.
  tff->setTmpdir(tff->genDirname());
  //
  test_2_helper(tff,5);
  test_2_helper(tff,5);
  //
  delete tff;
}

void
test_4()
{
  // generate 10 dirs for fun.
  for (int i=0;i<10;i++) {
    GlobalTmpFileFactory()->genDirname();
  }
  // set the dir to /var/tmp on unix. (Where on windows?)
  GlobalTmpFileFactory()->setTmpdir(GlobalTmpFileFactory()->getSystemTmpDir());
  // gen a subdir in that directory and set our tmp dir to dir we made.
  GlobalTmpFileFactory()->setTmpdir(GlobalTmpFileFactory()->genDirname());

  // generate 10 filenames in the new tmp dir for fun.
  for (int i=0;i<10;i++) {
    test_write_data(GlobalTmpFileFactory()->genFilename());
  }
}

//////////

int
main(int argc,char* argv[])
{
  PgOptions* opts=new PgOptions();
  //
  opts->setUsage("Example and test program for generating test filenames."
                 "\n"
                 );
  //
  opts->defineOption("h","help", PgOpt::BOOL_OPT,
                     "Show help for the program.",
                     "false");
  opts->defineOption("v","verbose",PgOpt::INT_OPT,
                     "verbose level",
                     "0");
  opts->defineOption("d","debug",PgOpt::INT_OPT,
                     "debugging level.\n"
                     "0 => None.\n"
                     "1 => Print the tmp filename as they are generated.\n"
                     "2 => Dont remove tmp files, leave them there for analysis.\n",
                     "0");
  //
  opts->parseArgv(argv);
  //
  opt_debug  =opts->getInt("debug");
  opt_verbose=opts->getInt("verbose");

  // set some options.  (this is optional.)
  GlobalTmpFileFactory()->setPrefix("tff-global-");
  GlobalTmpFileFactory()->setDebug(opt_debug);
  GlobalTmpFileFactory()->setVerbose(opt_verbose);

  //
  test_1();
  test_2();
  test_3();
  test_4();

  // need to clean up when exiting.
  GlobalTmpFileFactoryFree();
  //
  printf("ok.\n");
}

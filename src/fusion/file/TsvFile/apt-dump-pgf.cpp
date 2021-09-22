////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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
/// @file   apt-dump-pgf.cpp
/// @brief  Main for dumping pgf information.
*/

#include "file/TsvFile/DumpPgf.h"
#include "util/AptVersionInfo.h"
//

using namespace std;


int main (int argc,const char* argv[])
{
  try {
    const string version = AptVersionInfo::versionToReport();


    try
    {
        dumpPgf pgfDump (argv, version);
        pgfDump.run();
    }
    
    catch (exception& e)
    {
        // cerr << e.what() << endl; // errAbort() writes the error message.
        return 1;
    }
    return 0;
  } 
  catch(...) {
      Verbose::out(1,"Unexpected Error: uncaught exception.");
      return 1;
  }
  return 1;
}

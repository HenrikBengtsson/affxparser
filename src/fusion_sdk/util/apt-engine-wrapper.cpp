////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License (version 2) as 
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program;if not, write to the 
// 
// Free Software Foundation, Inc., 
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
////////////////////////////////////////////////////////////////

//
#include "util/BaseEngine.h"
#include "util/Engines.h"
#include "util/LogStream.h"
#include "util/PgOptions.h"
#include "util/Util.h"
//
#include <cstring>
#include <list>
#include <string>
#include <vector>
//
#ifndef WIN32
#include <unistd.h>
#endif /* WIN32 */

/**
\page apt-engine-wrapper MANUAL: apt-engine-wrapper (NON-OFFICIAL-RELEASE)

\section Contents Contents

<ul>
<li><a href="#intro">Introduction.</a></li>
<li><a href="#options">Program Options - command line options.</a></li>
</ul>

<a name="intro">
\section intro Introduction

This program allows you to directly call an analysis engine. You essentially specify the
engine and then all the engine parameters. You can get help on using the wrapper like this:

\verbatim
   apt-engine-wrapper --help
\endverbatim

This will also provide a listing of known analysis engines. You can get help on what options 
are available to each engine like so:

\verbatim
   apt-engine-wrapper --engine ProbesetSummarizeEngine --help
\endverbatim

And you can actually run the engine like this:

\verbatim
   apt-engine-wrapper --engine ProbesetSummarizeEngine -- [engine options]
\endverbatim


<a name="options">
\subsection manualOptions Options:

<!-- Do not edit this verbatim section.
     It will be replaced automatically w/ the results
     of running this program with the -h option
     --->

\verbatim
apt-engine-wrapper - Generic wrapper around the engine
factory. You probably want to use one of the engine
specific wrappers. Use ' -- ' to separate wrapper args
from engine args.

usage:
   apt-engine-wrapper --help 
   apt-engine-wrapper --engine ProbesetSummarizeEngine --help 
   apt-engine-wrapper --engine ProbesetSummarizeEngine -- \
     --cdf-file=... 



options:
     --engine                   The engine to run. [default ''] 
     --log-file The file to write out log items to. [default ''] 
   -h, --help This message. [default 'false'] 
     --version Display version information. [default 'false'] 
     --verbose Verbosity Level. [default '1'] 

Available Engines:
   MAS5Engine
   GenoQC
   ProbesetGenotypeEngine
   ProbesetSummarizeEngine
   SummaryGenotypeEngine
   CanaryEngine
   DmetCHPWriter
   DmetCopyNumberEngine
   DmetEngine
   CNAnalysisEngine
   CNCytoEngine
   CNLog2RatioEngine
   CNReferenceEngine
   CNWorkflowEngine
   TranslationEngine

version: NON-OFFICIAL-RELEASE-$Id: apt-engine-wrapper.cpp,v 1.24.2.1 2009/10/13 14:18:52 awilli Exp $
\endverbatim

*/

using namespace std;

/** Everybody's favorite function... */
int main(int argc, char *argv[]) {
  ofstream logOut;
  LogStream log;
  string logName;
  bool closeLog = false;
    
  try {
  const string version ("NON-OFFICIAL-RELEASE");
  const string cvsId ("$Id: apt-engine-wrapper.cpp,v 1.24.2.1 2009/10/13 14:18:52 awilli Exp $");
  const string versionToReport = version + "-" + cvsId;

  /* Parse options. */
  PgOptions opts;
  opts.setUsage("apt-engine-wrapper - Generic wrapper around the engine\n"
                "factory. You probably want to use one of the engine\n"
                "specific wrappers. Use ' -- ' to separate wrapper args\n"
                "from engine args.\n\n"
                "usage:\n"
                "   apt-engine-wrapper --help \n"
                "   apt-engine-wrapper --engine ProbesetSummarizeEngine --help \n"
                "   apt-engine-wrapper --engine ProbesetSummarizeEngine -- \\\n"
                "     --cdf-file=... \n\n");
  opts.defineOption("", "engine", PgOpt::STRING_OPT,
                    "The engine to run.",
                    "");
  opts.defineOption("", "log-file", PgOpt::STRING_OPT,
                    "The file to write out log items to.",
                    "");
  opts.defineOption("h", "help", PgOpt::BOOL_OPT,
                    "This message.",
                    "false");
  opts.defineOption("", "version", PgOpt::BOOL_OPT,
                    "Display version information.",
                    "false");
  opts.defineOption("", "verbose", PgOpt::INT_OPT,
                    "Verbosity Level.",
                    "1");

  int argvPos = opts.parseArgv(argv);
  const string progName = Util::fileRoot(opts.getProgName());

  // does the user want the version 
  if(opts.getBool("version")) {
    cout << "version: " << versionToReport << endl;
    exit(0);
  }
  // Do we need help? (I know I do...)
  else if(opts.getBool("help") || argc == 1) {
    set<string> toHide;
    opts.usage(toHide, true);

    if(opts.get("engine")!= "") {
        BaseEngine *engine = EngineReg::CreateEngine(opts.get("engine"));
        if(engine == NULL)
            Err::errAbort("Unable to create engine named '"+opts.get("engine")+"'");
        engine->optionUsage(toHide, true);
    } else {
        list<string> engineNames = EngineReg::GetEngineNames();
        cout << endl << "Available Engines:" << endl;
        for(list<string>::iterator iter = engineNames.begin(); iter != engineNames.end(); iter++) {
            cout << "   " << *iter << endl;
        }
    }
    cout << endl << "version: " << versionToReport << endl;
    exit(0);
  }
  else if(opts.get("engine")!= "") {
        BaseEngine *engine = EngineReg::CreateEngine(opts.get("engine"));
        if(engine == NULL)
            Err::errAbort("Unable to create engine named '"+opts.get("engine")+"'");

        /* Set up the logging and message handlers. */
        Verbose::setLevel(opts.getInt("verbose"));
        if(opts.get("log-file") != "") {
            string logName = opts.get("log-file");
            Util::mustOpenToWrite(logOut, logName.c_str());
            log.setStream(&logOut);
            Verbose::pushMsgHandler(&log);
            Verbose::pushProgressHandler(&log);
            Verbose::pushWarnHandler(&log);
            closeLog = true;
        }

        engine->parseArgv(argv,argvPos+1);

        engine->run();
  }
  } 
  catch(...) {
      Verbose::out(1,"Unexpected Error: uncaught exception.");
      // Close log files
      if(closeLog) {
          logOut.close();
      }
      return 1;
  }
  // Close log files
  if(closeLog) {
     logOut.close();
  }

  return 0;
}

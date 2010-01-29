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

#include "util/BaseEngine.h"
//
#include "util/AffxConv.h"
//
#include "calvin_files/utils/src/StringUtils.h"
//

/*
 * The head of the engine data objects.
 */
EngineReg *EngineReg::m_Head = NULL;

/*
 * Set the members. The head is the pointer to this class.
 */
EngineReg::EngineReg(const std::string &engName) : m_Next(m_Head)
{
    name = engName;
    m_Head = this;
}

/*
 * Destroy the class.
 */
EngineReg::~EngineReg()
{
}

/*
 * Create an engine based on the name.
 */
BaseEngine *EngineReg::CreateEngine(const std::string &name)
{
    // Find the matching engine data object.
    for (EngineReg *p=m_Head; p != NULL; p = p->m_Next)
    {
        if (p->name == name)
        {
            BaseEngine *engine = p->MakeObject();
            if (engine)
            {
                return engine;
            }
        }
    }
    return NULL;
}

/*
 * Gets the list of all registered engines.
 */
std::list<std::string> EngineReg::GetEngineNames()
{
    std::list<std::string> names;
    for (EngineReg *p=m_Head; p != NULL; p = p->m_Next)
    {
        names.push_back(p->name);
    }
    return names;
}


BaseEngine::BaseEngine() : m_OptionsChecked(false), m_DiskChecked(false), m_created_new_tempdir(false) {

    defineOptions();

    /* Track our memory usage */
    uint64_t freeRam = 0, totalRam = 0, swapAvail = 0, memAvail = 0;
    Util::memInfo(freeRam, totalRam, swapAvail, memAvail, false);
    setOpt("free-mem-at-start",ToStr(memAvail));
    Verbose::out(3,"In Base Engine Constructor");
}

BaseEngine::BaseEngine( char * argv[] ) : m_OptionsChecked(false), m_DiskChecked(false), m_created_new_tempdir(false) {
    Options::parseArgv( argv );
}

BaseEngine::~BaseEngine() {
    Verbose::out(3,"In Base Engine Destructor");
}
 
std::string BaseEngine::getProgName() { 
    return Options::getProgName(); 
}

void BaseEngine::setLibFileOpt(const std::string &option) {
    setOpt(option,Util::findLibFile(getOpt(option), getOpt("analysis-files-path")));
}

void BaseEngine::checkOptions() {

    if(m_OptionsChecked)
        return;

    // Note what options we started with
    snapshotOptions("initial");

    // Define additional state values -- those configured by base engine
    // The children will cal their defineStates() in checkOptionsImp()
    BaseEngine::defineStates();

    if(getOptBool("throw-exception")) { 
        Err::setThrowStatus(true); 
    }

    // does the user want the version 
    if(getOptBool("version")) {
        std::cout << "version: " << getOpt("version-to-report") << std::endl;
        exit(0);
    }
    // Do we need help? (I know I do...)
    else if(getOptBool("help")) {
        std::set<std::string> toHide;
        optionUsage(toHide, true);

        extraHelp();

        std::cout << "version: " << getOpt("version-to-report") << std::endl;
        exit(0);
    }

    checkOptionsImp();

    m_OptionsChecked = true;

    if(getOptBool("throw-exception")) { 
        ErrHandler *handler = Err::popHandler();
        Freez(handler);
    }
}

void BaseEngine::checkDiskSpace() {

    checkOptions();
    if(m_DiskChecked) 
        return;

    if(getOptBool("throw-exception")) { 
        Err::setThrowStatus(true); 
    }

    checkDiskSpaceImp();

    m_DiskChecked = true;

    if(getOptBool("throw-exception")) { 
        ErrHandler *handler = Err::popHandler();
        Freez(handler);
    }
}


/** 
 * @brief Top level function to open up files and make sure things look ok then
 * start analysis pathway.
 */
void BaseEngine::run() {

  if(getOptBool("throw-exception")) { 
      Err::setThrowStatus(true); 
  }

  Verbose::out(1, "Running " + getEngineName() + "...");
  printEngineOptions("Initial ");

  /* Check our options */
  checkOptions();
  checkDiskSpace();

  /* Set some initial state info */
  setOpt("time-start",Util::getTimeStamp());
  time_t startTime = time(NULL);

  /* Do the analysis requested. */
  Verbose::out(3,"Base Engine Before runImp()");
  Util::pushMemFreeAtStart();
  runImp();
  Util::popMemFreeAtStart();
  Verbose::out(3,"Base Engine After runImp()");

  setOpt("time-end",Util::getTimeStamp());
  time_t endTime = time(NULL);
  int t = int(  (float)(endTime - startTime) / 60.0 * 100); // convert to minutes
  setOpt("time-run-minutes",ToStr((float)t/100));
  Verbose::out(1, ToStr("Run took approximately: ") + ToStr((float)t/100) + ToStr(((float)t/100) > 1 ? " minutes." : " minute."));

  printEngineOptions("Final ");
  Verbose::out(1, "Done running " + getEngineName() + ".");

  if(getOptBool("throw-exception")) { 
      ErrHandler *handler = Err::popHandler();
      Freez(handler);
  }
} 

void BaseEngine::defineOptions() {
  defineOptionSection("Common Options (not used by all programs)");
  defineOption("h", "help", PgOpt::BOOL_OPT,
                     "Display program options and extra documentation about possible analyses. "
                     "See -explain for information about a specific operation.",
                     "false");
  defineOption("v", "verbose", PgOpt::INT_OPT,
                     "How verbose to be with status messages 0 - quiet, 1 - usual messages, 2 - more messages.",
                     "1");
  defineOption("", "version", PgOpt::BOOL_OPT,
                     "Display version information.",
                     "false");
  defineOption("f", "force", PgOpt::BOOL_OPT,
                     "Disable various checks including chip types. Consider using --chip-type option rather than --force.",
                     "false");
  defineOption("", "throw-exception", PgOpt::BOOL_OPT,
                     "Throw an exception rather than calling exit() on error. Useful for debugging. This option is intended for command line use only. If you are wrapping an Engine and want exceptions thrown, then you should call Err::setThrowStatus(true) to ensure that all Err::errAbort() calls result in an exception.",
                     "false");
  defineOption("", "analysis-files-path", PgOpt::STRING_OPT, 
                     "Search path for analysis library files. Will override AFFX_ANALYSIS_FILES_PATH environment variable.", 
                     "");
  defineOption("", "xml-file", PgOpt::STRING_OPT, 
                     "Input parameters in XML format (Will override command line settings).", 
                     "");
  defineOption("","temp-dir", PgOpt::STRING_OPT,
                     "Directory for temporary files when working off disk. Using network mounted drives is not advised. When not set, the output folder will be used. The defaut is typically the output directory or the current working directory.",
                     "");
  defineOption("o","out-dir", PgOpt::STRING_OPT,
                     "Directory for output files. Defaults to current working directory.",
                     ".");
  defineOption("","log-file", PgOpt::STRING_OPT,
                     "The name of the log file. Generally defaults to the program name in the out-dir folder.",
                     "");

  defineOptionSection("Engine Options (Not used on command line)");
  defineOption("","command-line", PgOpt::STRING_OPT,
                     "The command line executed.",
                     "");
  defineOption("","exec-guid", PgOpt::STRING_OPT,
                     "The GUID for the process.",
                     "");
  defineOption("","program-name", PgOpt::STRING_OPT,
                     "The name of the program",
                     "");
  defineOption("","program-company", PgOpt::STRING_OPT,
                     "The company providing the program",
                     "");
  defineOption("","program-version", PgOpt::STRING_OPT,
                     "The version of the program",
                     "");
  defineOption("","program-cvs-id", PgOpt::STRING_OPT,
                     "The CVS version of the program",
                     "");
  defineOption("","version-to-report", PgOpt::STRING_OPT,
                     "The version to report in the output files.",
                     "");
  defineOption("", "free-mem-at-start", PgOpt::STRING_OPT,
                     "How much physical memory was available when the engine run started.",
                     "0");
}

void BaseEngine::defineStates() {
  defineOption("", "time-start", PgOpt::STRING_OPT,
                     "The time the engine run was started",
                     "");
  defineOption("", "time-end", PgOpt::STRING_OPT,
                     "The time the engine run ended",
                     "");
  defineOption("", "time-run-minutes", PgOpt::STRING_OPT,
                     "The run time in minutes.",
                     "");
  defineOption("","analysis-guid", PgOpt::STRING_OPT,
                     "The GUID for the analysis run.",
                     "");
}


void BaseEngine::makeTempDir(std::string temp_dir) {
  if (!Util::directoryWritable(temp_dir)) {
    // Util::makeDir returns true if "temp-dir" is created, or false if
    // "temp-dir" already existed.  otherwise, fatal error
    m_created_new_tempdir = Util::makeDir(temp_dir);
    if (!m_created_new_tempdir) {
      Err::errAbort("Can't make or write to directory: " + temp_dir);
    }
  }
}

void BaseEngine::removeTempDir(std::string temp_dir) {
  if (m_created_new_tempdir && Util::directoryWritable(temp_dir)) {
    Util::dirRemove(temp_dir);
  }
}

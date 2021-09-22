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
#include "util/BaseEngine.h"
//
#include "calvin_files/utils/src/StringUtils.h"
#include "util/AffxConv.h"
#include "util/AptVersionInfo.h"
#include "util/Fs.h"
#include "util/MsgSocketHandler.h"

using namespace std;

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
    m_SocketHandler = NULL;
    /* Track our memory usage */
    uint64_t freeRam = 0, totalRam = 0, swapAvail = 0, memAvail = 0;
    Util::memInfo(freeRam, totalRam, swapAvail, memAvail, false);
    setOpt("free-mem-at-start",ToStr(memAvail));
    Verbose::out(3,"In Base Engine Constructor");
}

BaseEngine::BaseEngine( char * argv[] ) : m_OptionsChecked(false), m_DiskChecked(false), m_created_new_tempdir(false) {
    m_SocketHandler = NULL;
    parseArgv(argv);
}

BaseEngine::~BaseEngine() {
  Freez(m_SocketHandler);
  Verbose::out(3,"In Base Engine Destructor");
}
 
/**
 * Parse and set options from argv
 *
 * @param argv - arg vector
 */
int BaseEngine::parseArgv( const char * const * const argv, int start ) {
  int rv = Options::parseArgv( argv, start );
    if (getOptBool("console-off")) {
      Verbose::Param p = Verbose::getParam(); // To trigger setting up defaults.
      Verbose::removeDefault();
    }
    if (!getOpt("use-socket").empty()) {
      Verbose::out(1, "Setting up socket for reporting.");
      string url = getOpt("use-socket");
      string host = url.substr(0, url.find(':'));
      string port = url.substr(url.find(':') + 1);
      int verbosity = getOptInt("verbose");
      m_SocketHandler = new MsgSocketHandler(verbosity);
#ifndef _MSC_VER
      m_SocketHandler->setEndOfLine("\n");
#endif
      m_SocketHandler->openSocket(host, port);
      Verbose::pushMsgHandler(m_SocketHandler);
      Verbose::pushProgressHandler(m_SocketHandler);
	  Err::configureErrHandler(true, true, true, 0);
    }
    return rv;
}

std::string BaseEngine::getProgName() { 
    return Options::getProgName(); 
}

void BaseEngine::setLibFileOpt(const std::string &option) {
    setOpt(option,Fs::findLibFile(getOpt(option), getOpt("analysis-files-path")));
}


void BaseEngine::openStandardLog(const std::string& log_name,
                                 std::ofstream& log_ofstream,
                                 LogStream& log_logstream)
{
  if (!Fs::isWriteableDir(getOpt("out-dir"))) {
    if (Fs::mkdirPath(getOpt("out-dir")) != APT_OK) {
      Err::errAbort("Can't make or write to directory: '"+getOpt("out-dir")+"'");
    }
  }

  std::string log_path;

  if (getOpt("log-file") != "") {
    log_path = getOpt("log-file");
  }
  else {
    log_path = Fs::join(getOpt("out-dir"),log_name);
  }
  Fs::mustOpenToWrite(log_ofstream, log_path);
  log_logstream.setStream(&log_ofstream);
  Verbose::pushMsgHandler(&log_logstream);
  Verbose::pushProgressHandler(&log_logstream);
  Verbose::pushWarnHandler(&log_logstream);
}

std::vector<std::pair<std::string, std::string> >  BaseEngine::getMetaDataDescription() {
  vector<pair<string, string> >  pairVec;
  PgOpt *meta = getPgOpt("meta-data-info");

  for (int i = 0; i < meta->getValueCount(); i++) {
    pair<string, string> p;
    string param = meta->getValue(i);
    size_t pos = param.find('=');
    
    if (pos == string::npos) {
      Err::errAbort("meta-data-info value not in key=value pair: '" + param + "'");
    }
    
    p.first = param.substr(0,pos);
    p.second = param.substr(pos+1);

    if (p.first.length() == 0) {
      Err::errAbort("meta-data-info value has empty key in key=value pair: '" + param + "'");
    }
    if (p.second.length() == 0) {
      Err::errAbort("meta-data-info value has empty value in key=value pair: '" + param + "'");
    }
    // APT-510: we dont want to enforce this as it prevents having "="s in filenames.
    // if (p.second.find('=') != string::npos) {
    //   Err::errAbort("meta-data-info value has multiple '=' delimiters in key=value pair: '" + param + "'");
    // }
    pairVec.push_back(p);
  }
  return pairVec;
}

void BaseEngine::checkOptions() {
   
  /* @todo commenting out until implementation finished
    setVersionOptions();
    if (getOptBool("report-version-information"))
    {
        printEngineOptions("ReportVersionInformation ");
        Err::errAbort("The --report-version-information has been invoked.  This will give you a listing of all options, including version data, and then cause the program to exit.  To restore normal functionality remove this option from the command line input."  );
    }
  */
    if (m_OptionsChecked)
        return;

    // Note what options we started with
    snapshotOptions("initial");

    // Define additional state values -- those configured by base engine
    // The children will cal their defineStates() in checkOptionsImp()
    BaseEngine::defineStates();

    if (getOptBool("throw-exception")) { 
        Err::setThrowStatus(true); 
    }

    // does the user want the version 
    if (getOptBool("version")) {
        std::cout << "version: " << getOpt("version-to-report") << std::endl;
        exit(0);
    }
    // Do we need help? (I know I do...)
    else if (getOptBool("help")) {
        std::set<std::string> toHide;
        optionUsage(toHide, true);

        extraHelp();

        std::cout << "version: " << getOpt("version-to-report") << std::endl;
        exit(0);
    }

    checkOptionsImp();

    m_OptionsChecked = true;

    if (getOptBool("throw-exception")) { 
        ErrHandler *handler = Err::popHandler();
        Freez(handler);
    }
}

/* @todo commenting out until implementation finished
void BaseEngine::setVersionOptions(){
    AptVersionInfo  aptVersionInfo; 
    setOpt("BAMBOO_BUILD", aptVersionInfo.reportBambooBuild());
    setOpt("COMPILE_CC_VERSION", aptVersionInfo.reportCompileCCVersion());
    setOpt("COMPILE_DATE", aptVersionInfo.reportCompileDate());
    setOpt("COMPILE_HOST", aptVersionInfo.reportCompileHost());
    setOpt("COMPILE_OS", aptVersionInfo.reportCompileOS());
    setOpt("RELEASE", aptVersionInfo.reportRelease());
    setOpt("SVN_VERSION", aptVersionInfo.reportSVNVersion());
    setOpt("SVN_URL", aptVersionInfo.reportSVNVersion());
}
*/

void BaseEngine::checkDiskSpace() {

    checkOptions();
    if (m_DiskChecked) 
        return;

    if (getOptBool("throw-exception")) { 
        Err::setThrowStatus(true); 
    }

    checkDiskSpaceImp();

    m_DiskChecked = true;

    if (getOptBool("throw-exception")) { 
        ErrHandler *handler = Err::popHandler();
        Freez(handler);
    }
}


/** 
 * @brief Top level function to open up files and make sure things look ok then
 * start analysis pathway.
 */
void BaseEngine::run() {

  if (getOptBool("throw-exception")) { 
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

  if (getOptBool("throw-exception")) { 
      ErrHandler *handler = Err::popHandler();
      Freez(handler);
  }
  if (m_SocketHandler != NULL) {
    Verbose::removeMsgHandler(m_SocketHandler);
    Verbose::removeProgressHandler(m_SocketHandler);
    m_SocketHandler->finishedMsg();
    Freez(m_SocketHandler);
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
  defineOption("", "console-off", PgOpt::BOOL_OPT,
                     "Turn off the default messages to the console but not logging or sockets.",
                     "false");
  defineOption("", "use-socket", PgOpt::STRING_OPT,
                     "Host and port to print messages over in localhost:port format",
                     "");
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
  defOptMult("","meta-data-info", PgOpt::STRING_OPT,
                "Meta data in key=value pair that will be output in headers.",
                "");

  /* @todo commenting out until implementation finished 
  defineOption("", "report-version-information", PgOpt::BOOL_OPT,
                     "Reports version information, command line options and exits.",
                     "false");
  // The values that the following states are set to are created during the build process.  
  // The information is initilized from environment variables in sdk/Makefile.defs and passed to the compiler
  // with the -D compiler option. The preprocessor run then sets them within the AptVersionInfo.h file
  // to the values set in the Makefile.defs.  They are then available to the apt code base via the access 
  // functions defined in AptVersionInfo.h 
  // Note that all the names of the states are the names given in the #define's in the AptVersionInfo.h file
  // with the APT_VER prefix removed.  This avoids any preprocessor overwrites.  
  defineOption("","BAMBOO_BUILD", PgOpt::STRING_OPT,
                     "The bamboo version used while building the binary",
                     "");
  defineOption("","COMPILE_CC_VERSION", PgOpt::STRING_OPT,
                     "The version of the compiler used while building the binary.",
                     "");
  defineOption("","COMPILE_DATE", PgOpt::STRING_OPT,
                     "The date on which the binary was built.",
                     "");
  defineOption("","COMPILE_HOST", PgOpt::STRING_OPT,
                     "The host on which the binary was built.",
                     "");
  defineOption("","COMPILE_OS", PgOpt::STRING_OPT,
                     "The operating system on which the binary was built.",
                     "");
  defineOption("","RELEASE", PgOpt::STRING_OPT,
                     "The apt version eg. 1.12.0 set when the binary was built.",
                     "");
  defineOption("","SVN_VERSION", PgOpt::STRING_OPT,
                     "The version number of the SVN repository from which the binary was built.",
                     "");
  defineOption("","SVN_URL", PgOpt::STRING_OPT,
                     "The URL at which the SVN repository is rooted.",
                     "");
  */
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

//////////

// @todo this is wrong "m_created_new_tempdir" is object state,
//       but the temp_dir path is being passed in;
//       temp_dir should be state as well.
void BaseEngine::makeTempDir(std::string temp_dir) {
  m_created_new_tempdir=false;

  if (Fs::dirExists(temp_dir)==false) {
    m_created_new_tempdir=true;
    Fs::ensureWriteableDirPath(temp_dir);
  }
  if (Fs::isWriteableDir(temp_dir)==false) {
    Err::errAbort("Can't make or write to directory: "+FS_QUOTE_PATH(temp_dir));
  }
}

void BaseEngine::removeTempDir(std::string temp_dir) {
  if (m_created_new_tempdir) {
    Fs::rmdir(temp_dir);
  }
}

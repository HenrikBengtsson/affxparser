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

/**
 * @file   RegressionTest.h
 * @author Chuck Sugnet
 * @date   Sun Dec  4 23:26:39 2005
 * 
 * @brief  Utility class for helping to do regression testing.
 */
#ifndef REGRESSION_TEST_H
#define REGRESSION_TEST_H

#include <assert.h>
#include <string>
#include <stdlib.h>
#include "Util.h"
#include "Verbose.h"
#include "RegressionCheck.h"
#include "MatrixCheck.h"
#include "MixedFileCheck.h"

/**
 * Utility class for helping to do regression testing.
 */
class RegressionTest {

public:

  /** Constructor. */
  RegressionTest(const char *generatedFile, const char *goldFile,
                 double epsilon, const char *command, int rowSkip,
                 int colSkip, bool matchNames,
                 int allowedMisMatch) {
    m_Checks.push_back(new MatrixCheck(generatedFile, goldFile, epsilon,
                                       rowSkip, colSkip, matchNames, allowedMisMatch));
    m_CommandStr = command;
    m_CommandStr = Util::getPathName(m_CommandStr.c_str());
    m_Delete = true;
  }
  
  RegressionTest(const char *command, const std::vector<RegressionCheck *> &fileChecks, bool doDelete=true) {
    m_CommandStr = command;
    m_CommandStr = Util::getPathName(m_CommandStr.c_str());
    m_Checks = fileChecks;
    m_Delete = doDelete;
  }

  ~RegressionTest() {
    if(m_Delete) {
      for(uint32_t i = 0; i < m_Checks.size(); i++) {
        delete m_Checks[i];
      }
    }
  }
  
  /** 
   * Run the command to check the test. 
   * @return true if executes sucessfully, false otherwise.
   */
  bool run() {
    bool success = true;
    assert(m_CommandStr != "");
    int retVal = system(m_CommandStr.c_str());
    if(retVal == -1) {
      m_Error += ToStr(" Failed to run command: ") + m_CommandStr;
      success = false;
    }
    if(retVal != 0) {
      m_Error += ToStr(" Command exited with non-zero status: ") + m_CommandStr;
      success = false;
    }
    return success;
  }

  /** 
   * Do we pass our post-run tests??? 
   * @return true if we pass our test
   */
  bool pass() {
    bool success = true;
    if(run()) {
      for(unsigned int i = 0; i < m_Checks.size(); i++) {
        std::string msg;
        bool currentPass = m_Checks[i]->check(msg);
        if(!currentPass) {
          m_Error += msg;
          success = false;
        }
      }
    }
    /* failed just running the test. */
    else {
      success = false;
    }
    return success;
  }

  /** 
   * Get whatever message was reported. 
   *
   * @return the concatanation of any error messages that have been
   * sent.
   */
  std::string getErrorMsg() { return m_Error; }

  /** 
   * Try to link and if necessary copy a directory from another place.
   * @param targetName - path to directory to be synced
   * @param localName - where we want data synced to locally.
   * @param hostUrl - url for data if copying necessary.
   * @param errorMsg - fill in error message here if something goes wrong.
   * 
   * @return - true if sucessful, false otherwise.
   */  
  static bool syncDirectory(const char *targetName, const char *localName, 
                            const char *hostUrl, std::string &errorMsg) {
    std::string targetPath(targetName);
    std::string localPath(localName);
    targetPath = Util::getPathName(targetPath.c_str());
    localPath = Util::getPathName(localPath.c_str());
#if defined (WIN32)
    errorMsg = "Please copy data in" + targetPath + " to " + localPath;
    return false;
#else
    /* On unix, if possible try to just link the data as this is quick. */
    if(Util::directoryReadable(targetPath.c_str())) {
      int retVal = 0;
      std::string command = ToStr("ln -s ") + targetPath + "  " + localPath;
      retVal = system(command.c_str());
      if(retVal == 0) {
        return true;
      }
      else {
        errorMsg = "Failed trying to run command: " + command;
        return false;
      }
    }
    /* Can't see that directory, try to download it. */
    else {
      std::string command = ToStr("wget -m -P ") + localPath  + 
        " " + ToStr(hostUrl);
      int retVal = 0;
      Verbose::out(1, "Retrieving remote data using wget.", false);
      retVal = system(command.c_str());
      Verbose::out(1, "\tDone.");
      if(retVal == 0) {
        return true;
      }
      else {
        errorMsg = "Failed trying to run command: " + command;
      }
    }
#endif
    return false;
  }

private:
  RegressionTest() {}
  std::vector<RegressionCheck *> m_Checks;
  std::string m_CommandStr;     ///< Command that generates the generated file, platform adjusted.
  std::string m_Error;          ///< Any errors generated are stored here.
  bool m_Delete;
};

#endif /* REGRESSIONTEST_H */

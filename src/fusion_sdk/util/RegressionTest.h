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

#include "util/MatrixCheck.h"
#include "util/MixedFileCheck.h"
#include "util/RegressionCheck.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
//

/**
 * Utility class for helping to do regression testing.
 */
class RegressionTest {

public:

  /** Constructor. */
  RegressionTest(const std::string& name,
                 const std::string& generatedFile, 
                 const std::string& goldFile,
                 double epsilon, 
                 const std::string& command,
                 int rowSkip,
                 int colSkip,
                 bool matchNames,
                 int allowedMisMatch,
                 bool negTest = false)
  {
    m_Name = name;
    m_Checks.push_back(new MatrixCheck(generatedFile, goldFile, epsilon,
                                       rowSkip, colSkip, matchNames, allowedMisMatch));
    m_CommandStr = command;
    m_CommandStr = Util::convertPathName(m_CommandStr,false);
    m_Delete = true;
    m_NegTest = negTest;
  }
  
  RegressionTest(const std::string& name,
                 const std::string& command, 
                 const std::vector<RegressionCheck *>& fileChecks,
                 bool doDelete=true,
                 bool negTest = false)
  {
    m_Name = name;
    m_CommandStr = command;
    m_CommandStr = Util::convertPathName(m_CommandStr,false);
    m_Checks = fileChecks;
    m_Delete = doDelete;
    m_NegTest = negTest;
  }

  ~RegressionTest() {
    if (m_Delete) {
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

    time_t startTime = time(NULL);
    int retVal = system(m_CommandStr.c_str());
    time_t endTime = time(NULL);
    m_RunTime = endTime - startTime; // convert to ms
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
    std::string xmlFile = "JUnitTestResults." + m_Name + ".xml";
    std::ofstream xmlOut;
    std::vector<std::string> m_Results(m_Checks.size() + 1);

    if(Util::fileExists(xmlFile))
        Util::fileRemove(xmlFile);

    int numFailed = 0;
    int numSkipped = 0;
    int numPassed = 0;

    if(run()) {
      numPassed++;
      m_Results[0] = "  <testcase classname=\"" + m_Name + "\" name=\""+ m_Name +".run\" time=\""+ToStr(m_RunTime)+"\"/>\n";
      for(unsigned int i = 0; i < m_Checks.size(); i++) {
        std::string msg;
        bool currentPass;
        try {
            currentPass = m_Checks[i]->check(msg);
            if(m_Checks[i]->m_NegTest) {
                currentPass = !currentPass;
                msg = "test passed, but was not expected to pass";
            }
        } 
        catch(...) {
            currentPass = false;
            msg = "unexpected exception thrown";
        }
        if(!currentPass) {
          m_Error += msg;
          success = false;
          numFailed++;
          m_Results[i+1] = 
                     "  <testcase classname=\"" + m_Name + "\" name=\""+ m_Name + "." + m_Checks[i]->m_Name +"\">\n" +
                     "      <error message=\"check failed\">\n" + msg +
                     "      </error>\n" +
                     "  </testcase>\n";
        } else {
          numPassed++;
          m_Results[i+1] = "  <testcase classname=\"" + m_Name + "\" name=\""+ m_Name + "." + m_Checks[i]->m_Name +"\"/>\n";
        }
      }
    }
    /* failed just running the test. */
    else {
      std::string reportMsg;
      numSkipped = m_Checks.size();
      if(m_NegTest) {
        numFailed = 0;
        numPassed = 1;
        reportMsg = "test skipped because of expected execution failure -- this test should probably not exist";
        success = true;
        m_Results[0] = "  <testcase classname=\"" + m_Name + "\" name=\""+ m_Name +".run\"/>\n";
      } else {
        numFailed = 1;
        numPassed = 0;
        reportMsg = "test skipped because of unexpected execution failure";
        success = false;
        m_Results[0] = "  <testcase classname=\"" + m_Name + "\" name=\""+ m_Name +".run\">\n" +
                     "      <error message=\"execution failed\">\n" + m_Error + "\n" +
                     "      </error>\n" +
                     "  </testcase>\n";
      }
      for(int i = 0; i < m_Checks.size(); i++) {
          m_Results[i+1] = 
                     "  <testcase classname=\"" + m_Name + "\" name=\""+ m_Name + "." + m_Checks[i]->m_Name +"\">\n" +
                     "      <skipped message=\"" + reportMsg + "\"/>\n" + 
                     "  </testcase>\n";
      }

    }

    // Generate xml Junit file
    xmlOut.open(xmlFile.c_str());
    xmlOut << "<?xml version=\"1.0\"?>\n";
    xmlOut << "<testsuite errors=\"0\" skipped=\"" + ToStr(numSkipped) + "\" failures=\"" + ToStr(numFailed) + "\" tests=\"" + ToStr(m_Checks.size() + 1) + "\" name=\"" + m_Name + "\">\n";
    for(int i=0; i< m_Results.size(); i++)
        xmlOut << m_Results[i];
    xmlOut << "</testsuite>\n";
    xmlOut.close();

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
    targetPath = Util::convertPathName(targetPath);
    localPath = Util::convertPathName(localPath);
#if defined (WIN32)
    errorMsg = "Please copy data in" + targetPath + " to " + localPath;
    return false;
#else
    /* On unix, if possible try to just link the data as this is quick. */
    if(Util::directoryReadable(targetPath)) {
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

public:
  std::string m_Name;
  bool m_NegTest;

private:
  RegressionTest() {}
  std::vector<RegressionCheck *> m_Checks;
  std::string m_CommandStr;     ///< Command that generates the generated file, platform adjusted.
  std::string m_Error;          ///< Any errors generated are stored here.
  bool m_Delete;
  int64_t m_RunTime;
};

#endif /* REGRESSIONTEST_H */

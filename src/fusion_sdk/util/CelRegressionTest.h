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
 * @file   CelRegressionTest.h
 * @author Pete Klosterman
 * @date   Wed Mar 15 09:32:34 2006
 * 
 * @brief  Utility class for cel file based regression testing.
 */
#ifndef CEL_REGRESSION_TEST_H
#define CEL_REGRESSION_TEST_H

#include <string>
#include <stdlib.h>
#include "Util.h"
#include "Verbose.h"
#include "file/CELFileData.h"

/**
 * Utility class for cel file based regression testing.
 */
class CelRegressionTest {

public:

  /** 
   * Constructor.
   * 
   * @param goldFile - File believed to be "truth".
   * @param generatedFile - File created by application being tested.
   * @param epsilon - Maximum abosolute difference from truth acceptable.
   * @param command - Command to execute.
   * @param allowedMismatch - How many can we get wrong before failing?
   */
  CelRegressionTest (const char* goldFile, const char* generatedFile,
    const double epsilon, const char* command, const int allowedMismatch) :
    m_GoldFile (goldFile), m_GeneratedFile (generatedFile), m_Epsilon (epsilon),
    m_CommandStr (Util::getPathName (command)), m_AllowedMismatch (allowedMismatch) {}
  
  /** 
   * Run the command to check the test. 
   * @return true if executes sucessfully, false otherwise.
   */
  bool run () {
    bool success = true;
    assert (! m_CommandStr.empty());
    int retVal = system (m_CommandStr.c_str());
    if (retVal == -1) {
      m_Error = "Failed to run command: " + m_CommandStr;
      success = false;
    }
    if (retVal != 0) {
      m_Error = "Command exited with non-zero status: " + m_CommandStr;
      success = false;
    }
    return success;
  }

  /** 
   * Check that the generated and gold files are the same.
   * @param msg - Fills in an error message if the test fails, an empty string otherwise.
   * @return true if the same, false otherwise.
   */
  bool checkFiles (std::string& msg) {
    affxcel::CCELFileData goldCel;
    affxcel::CCELFileData generatedCel;
    goldCel.SetFileName (m_GoldFile);
    if (! goldCel.Read()) {
      msg = "Can't read cel file: " + std::string (m_GoldFile);
      return false;
    }
    generatedCel.SetFileName (m_GeneratedFile);
    if (! generatedCel.Read()) {
      msg = "Can't read cel file: " + std::string (m_GeneratedFile);
      return false;
    }
    const int numCells = goldCel.GetNumCells();
    int diffCount = 0;
    for (int celIx = 0; celIx < numCells; celIx++) {
      float diff = fabs (generatedCel.GetIntensity(celIx) - goldCel.GetIntensity(celIx));
      if (diff >= m_Epsilon)
	if (++diffCount > m_AllowedMismatch) {
	  msg = "Exceeded number of allowed differences at index " + ToStr (celIx);
	  // Quit now - don't bother comparing all of the intensities.
	  return false;
	}
    }
    return true;
  }

  /** 
   * Do we pass our post-run tests??? 
   * @return true if we pass our test
   */
  bool pass () {
    bool success = true;
    if (run ()) {
      std::string msg;
      bool currentPass = checkFiles (msg);
      if (!currentPass) {
	Verbose::out (1, "Error: " + msg);
        m_Error = "File: " + std::string (m_GeneratedFile) + " failed tests. ";
        success = false;
      }
    }
    /* failed just running the test. */
    else
      success = false;

    return success;
  }

  /** 
   * Get whatever message was reported. 
   *
   * @return the concatanation of any error messages that have been
   * sent.
   */
  std::string getErrorMsg () { return m_Error; }

private:
  const char* m_GoldFile;       ///< File believed to be "truth".
  const char* m_GeneratedFile;  ///< File created by application being tested.
  const double m_Epsilon;       ///< Maximum abosolute difference from truth acceptable.
  const std::string m_CommandStr; ///< Command that generates the generated file.
  const int m_AllowedMismatch;  ///< How many can we get wrong before failing?
  std::string m_Error;          ///< Any errors generated are stored here.
};

#endif /* CEL_REGRESSIONTEST_H */

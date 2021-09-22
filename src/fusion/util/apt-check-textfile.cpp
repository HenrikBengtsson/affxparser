////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Affymetrix, Inc.
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
#include "util/PgOptions.h"
#include "util/RegressionCheck.h"
#include "util/TextFileCheck.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;
const int passed = 0;
const int failed = 1;

int main(int argc, char* argv[])
{
  try {

    //If no arguments given
    if(argc == 0) 
      {
	cout<<"Error: Incorrect number of arguments passed to Matrix Check"<<endl;
	return failed;
      } 
    else 
      {
	PgOptions* opts = new PgOptions();
      
	//Set Usage message for Text File Check
	opts->setUsage("\n\n*****apt-check-textfile Help*****");
      
	//Define acceptable Command Line arguments for apt-check-textfile
	opts->defineOption("h", "help", PgOpt::BOOL_OPT,
			   "A bool value, answer true if you want help!",
			   "false");		
	opts->defineOption("g", "gold", PgOpt::STRING_OPT,
			   "A String representing the Gold file to check against - the perfect/reference file",
			   "");
	opts->defineOption("n", "gen", PgOpt::STRING_OPT,
			   "A String representing the Generated File to check - the test file",
			   "");
	opts->defineOption("s", "skipLines", PgOpt::INT_OPT,
			   "An integer that signifies the number of lines to skip before comparing",
			   "0");
	opts->defineOption("v", "expectedRetVal", PgOpt::INT_OPT,
			   "An integer value representing the desired return value used to flag a positive or negative test.  0 is positive, 1 is negative",
			   "0");
	opts->defineOption("t", "testName", PgOpt::STRING_OPT,
			   "Name of the Test being Run",
			   "");

	//Parse argv and store it within PgOptions class variables
	//Match the command line arguments to the proper options
	opts->parseArgv(argv);

	//DEBUG
	//opts->dump();

	if(opts->getBool("help") == true)
	  {
	    opts->usage();
	    return passed;
	  }

	std::string gen;
	std::string gold;
	if(opts->get("gen") != "")
	  {
	    gen = opts->get("gen");
	  }
	else
	  {
	    cout<<"Error: No value for Generated File (gen) given.  Exiting"<<endl;
	    return failed;
	  }

	if(opts->get("gold") != "")
	  {
	    gold = opts->get("gold");
	  }
	else
	  {
	    cout<<"Error: No value for Gold File (gold) given.  Exiting"<<endl;
	    return failed;
	  }

	//Store variable values
	int skipLines = opts->getInt("skipLines");
	int expectedRetVal = opts->getInt("expectedRetVal");
	std::string testName = opts->get("testName");

	//Run Check
	TextFileCheck* tCheck;
	tCheck = new TextFileCheck(gen, gold, skipLines);
	std::string errorMsg = "";
	bool pass =  tCheck->check(errorMsg);

	//Test for Regular Pass (Positive)
	if(expectedRetVal == 0)
	  {
	    if(!pass)
	      {
		cout<<"Error in " + testName + "(): " + errorMsg<<endl;
		cout<<"Error: Failed Text File Check"<<endl;
		return failed;
	      }
	    else
	      {
		cout<<"Passed Text File Check"<<endl;
		return passed;
	      } 
	  }
	//Test for Negative
	else
	  {
	    if(pass)
	      {
		cout<<"Error in " + testName + "(): " + errorMsg<<endl;
		cout<<"Error: Failed Text File Check"<<endl;
		return failed;
	      }
	    else
	      {
		cout<<"Passed Text File Check"<<endl;
		return passed;
	      } 
	  }
      }
  } 
  catch(...) 
    {
      Verbose::out(1,"Unexpected Error: uncaught exception.");
      return failed;
    }
  return failed;
}

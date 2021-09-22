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

#include "util/ChpCheck.h"
#include "util/PgOptions.h"
#include "util/RegressionCheck.h"
#include "util/Util.h"
#include "util/Verbose.h"
//
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
//

using namespace std;
const int passed = 0;
const int failed = 1;


int main(int argc, char* argv[])
{
  try {

    //If no arguments given
    if(argc == 0) 
      {
	cout<<"Error: Incorrect number of arguments passed to Chp Check"<<endl;
	return failed;
      } 
    else 
      {

	PgOptions* opts = new PgOptions();
      
	//Set Usage message for Chp Check
	opts->setUsage("\n\n*****apt-check-chp Help*****");
      
	//Define acceptable Command Line arguments for apt-check-chp
	opts->defineOption("h", "help", PgOpt::BOOL_OPT,
			   "A bool value, answer true if you want help!",
			   "false");
	opts->defineOption("g", "gold", PgOpt::STRING_OPT,
			   "A String representing theGold file to check against - the perfect/reference file",
			   "");
	opts->defineOption("n", "gen", PgOpt::STRING_OPT,
			   "A String representing the Generated File to check - the test file",
			   "");
	opts->defineOption("e", "epsilon", PgOpt::DOUBLE_OPT,
			   "A double that is the maximum accetable difference calculated",
			   "0.0001");
	opts->defineOption("p", "prefix", PgOpt::STRING_OPT,
			   "A string representing hte prefix for the header string alg part",
			   "apt-");
	opts->defineOption("d", "diffAllowed", PgOpt::INT_OPT,
			   "An integer representing the number of acceptable mismatches/errors before the check fails",
			   "0");
	opts->defineOption("b", "bCheckHeaders", PgOpt::BOOL_OPT,
			   "",
			   "true");
	opts->defineOption("v", "expectedRetVal", PgOpt::INT_OPT,
			   "An integer value representing the desired return value used to flag a positive or negative test.  0 is positive, 1 is negative",
			   "0");
	opts->defineOption("t", "testName", PgOpt::STRING_OPT,
			   "testName = Name of the Test being Run",
			   "");
	//Parse argv and store it within PgOptions class variables
	//Match the command line arguments to the proper options
	opts->parseArgv(argv);

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
	    cout<<"Error: No value for Generated Files (gen) given.  Exiting"<<endl;
	    return failed;
	  }

	if(opts->get("gold") != "")
	  {
	    gold = opts->get("gold");
	  }
	else
	  {
	    cout<<"Error: No value for Gold Files (gold) given.  Exiting"<<endl;
	    return failed;
	  }
      
	//Store variable values
	double eps = opts->getDouble("epsilon");
	int diffAllowed = opts->getInt("diffAllowed");
	std::string prefix = opts->get("prefix");
	bool bCheckHeaders = opts->getBool("bCheckHeaders");
	int expectedRetVal = opts->getInt("expectedRetVal");
	std::string testName = opts->get("testName");

	//Run Check
	ChpCheck* cCheck;
	cCheck = new ChpCheck(gen, gold, diffAllowed, prefix, eps, bCheckHeaders);
	string errorMsg = "";
	bool pass =  cCheck->check(errorMsg);

	//Test for Regular Pass (Positive)
	if(expectedRetVal == 0)
	  {
	    if(!pass)
	      {
		cout<<"Error in " + testName + "(): " + errorMsg<<endl;
		cout<<"Error: Failed Chp Check"<<endl;
		return failed;
	      }
	    else
	      {
		cout<<"Passed Chp Check"<<endl;
		return passed;
	      } 
	  }
	//Test for Negative
	else
	  {
	    if(pass)
	      {
		cout<<"Error in " + testName + "(): " + errorMsg<<endl;
		cout<<"Error: Failed Chp Check"<<endl;
		return failed;
	      }
	    else
	      {
		cout<<"Passed Chp Check"<<endl;
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


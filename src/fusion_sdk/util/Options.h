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

/**
 * @file   Options.h
 * @author Alan Williams
 * @date   Mon Jun 23 14:57:34 PDT 2008
 * 
 * @brief base class for tracking class options
 */

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

//
#include "portability/apt-win-dll.h"
#include "util/Options.h"
#include "util/PgOptions.h"
#include "util/Util.h"
//
#include <cstring>
#include <string>
#include <vector>

/**
 * @brief base class for tracking class options
 */
class APTLIB_API Options {

public:

  /**
   * Constructor
   */
  Options();

  /**
   * Constructor
   * @param argv - vector of args to parse
   */
  Options( char * argv[] );

  /**
   * Destructor
   */
  ~Options();

  /**
   * Get the name of the options set
   */
  std::string getProgName(int oI = 0);

 int getArgCount(int oI = 0) {
      return m_Options[oI].getArgCount();
  }
  std::string getArg(int index, int oI = 0){
      return m_Options[oI].getArg(index);
  }
  void getOptionNames(std::vector<std::string> &names, int oI = 0);
  void getOptionTypes(std::vector<PgOpt::PgOptType> &types, int oI = 0);
	
  std::string commandLine(int oI = 0) {
    return m_Options[oI].commandLine();
  }
  
  void setUsage(const std::string& usage) {
      m_Options[0].setUsage(usage);
  }

  PgOpt* defineOption(const std::string& shortName,
                      const std::string& longName,
                      PgOpt::PgOptType_t type,
                      const std::string& help,
                      const std::string& defaultVal) {
      return m_Options[0].defineOption(shortName, longName, type, help, defaultVal);
  }

  PgOpt* defOptMult(const std::string& shortName,
                      const std::string& longName,
                      PgOpt::PgOptType_t type,
                      const std::string& help,
                      const std::string& defaultVal) {
      return m_Options[0].defOptMult(shortName, longName, type, help, defaultVal);
  }

  void defineOptionSection(const std::string &sectionName) {
      m_Options[0].defineOptionSection(sectionName);
  }

  void optionsClear() {
      m_Options.resize(1);
      m_Options[0].clear();
  }


  void optionUsage(std::set<std::string> &hiddenOpts, bool printOpts = true, int oI = 0) {
      m_Options[oI].usage(hiddenOpts,printOpts);
  }

  /**
   * Check to see if an option is defined.
   * 
   * @param name - option name
   */
  bool isOptDefined(const std::string& name, int oI = 0);

  /**
   * Set option value
   * 
   * @param name - option name
   * @param value - value for the option
   */
  void setOpt(const std::string &name, const std::string& value);

  /**
   * Set option to vector of values
   * 
   * @param name - option name
   * @param value - vector of values
   */
  void setOpt (const std::string &name, std::vector< std::string > & values);

  /**
   * Set option to vector of vector of values
   * 
   * @param name - option name
   * @param value - vector of vector of values
   */
  void setOpt(const std::string & name, std::vector< std::vector<std::string> > & values );

  /**
   * Add a value to a vector of values for option
   *
   * @param name - option name
   * @param value - value to push onto vector of values
   */
  void pushOpt( const std::string &name, const std::string & value );

  /**
   * Add a vector of values to the values for an option
   *
   * @param name - option name
   * @param value - vector of values to push
   */
  void pushOpt ( const std::string & name, std::vector< std::string > values );

  /**
   * Parse and set options from argv
   *
   * @param argv - arg vector
   */
  virtual int parseArgv( const char * const * const argv, int start = 1 );

  /**
   * Get the underlying PgOpt containing information
   *
   * @param name - the name of the option
   */
  PgOpt* getPgOpt(const std::string& name, int oI = 0);
  
  /**
   * Get the value of an option
   *
   * @param name - the name of the option
   */
  std::string getOpt(const std::string& name, int oI = 0)
  {
    if (!isOptDefined(name, oI)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
    return m_Options[oI].get( name );
  }
  
  /**
   * Get the boolean value of an option
   *
   * @param name - the name of the option
   */
  bool getOptBool(const std::string& name, int oI = 0);

  /**
   * Get the integer value of an option
   *
   * @param name - the name of the option
   */
  int getOptInt(const std::string& name, int oI = 0);

  /**
   * Get the integer value of an option
   *
   * @param name - the name of the option
   */
  double getOptDouble(const std::string& name, int oI = 0);

  /**
   * Return a vector of values for a particular option
   * 
   * @param name - option name
   */
  std::vector< std::string >& getOptVector (const std::string &name, int oI = 0);
 
  void printOptions(const std::string &prefix, int oI = 0);

  std::string getXMLParameterFileName(int oI = 0);
  std::string getXMLParameterFileGuid(int oI = 0);

  void setOptions(PgOptions &opts) {
      for(unsigned int i=0; i<opts.m_option_vec.size(); i++)
          if(opts.m_option_vec[i]->isSet())
                m_Options[0].mustFindOpt(opts.m_option_vec[i]->m_longName)->m_values = opts.m_option_vec[i]->m_values;
  }

  int snapshotOptions() {
      std::string empty;
      return snapshotOptions(empty);
  }

  int snapshotOptions(const std::string &label){
      int index = int(m_Options.size());
      m_Options.push_back(m_Options[0]);
      if(label != "")
        m_Labels[label] = index;
      return index;
  };

private:
  /// Underlying options implementation
  std::vector<PgOptions> m_Options;
  std::map<std::string,int> m_Labels;
};


#endif /* _OPTIONS_H_ */

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

#include "util/Options.h"
//
#include "calvin_files/utils/src/StringUtils.h"

/**
 * Constructor
 */
Options::Options() { 
    m_Options.resize(1);
    m_Labels["current"] = 0;
}

/**
 * Destructor
 */
Options::~Options() { }

void Options::getOptionNames(std::vector<std::string> &names, int oI){ 
    for(size_t i=0; i < m_Options[oI].m_option_vec.size(); i++) {
        names.push_back(m_Options[oI].m_option_vec[i]->m_longName);
    }
}

void Options::getOptionTypes(std::vector<PgOpt::PgOptType> &types, int oI) {
    for(size_t i=0; i < m_Options[oI].m_option_vec.size(); i++) {
        types.push_back(m_Options[oI].m_option_vec[i]->m_type);
    }
}

/**
 * Check to see if an option is defined.
 * 
 * @param name - option name
 */
bool Options::isOptDefined(const std::string& name, int oI) 
{
  return (m_Options[0].findOpt(name) != NULL);
}

/**
 * Set option value
 * 
 * @param name - option name
 * @param value - value for the option
 */
void Options::setOpt(const std::string &name, const std::string& value) {
	if (!m_Options[0].isOptDefined(name)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
	m_Options[0].set(name, value);
  //Verbose::out(1,"Set string option " + name + " to " + value);
} 

/**
 * Set option to vector of values
 * 
 * @param name - option name
 * @param value - vector of values
 */
void Options::setOpt(const std::string &name, std::vector< std::string > & values) {
  if (!isOptDefined(name)) {
    Err::errAbort("Option " + name + " cannot be found in the options for this engine.");
  }
  if (values.size() != 0) {
    if (m_Options[0].mustFindOpt(name)->m_allowMultiple)
    {
	    m_Options[0].mustFindOpt(name)->m_values = values;
    }
    else
    {
	    if (values.size() > 1) {Err::errAbort("Option " + name + " does not allow multiple values.");}
	    if (values.size() == 1) {setOpt(name, values[0]);}
    }
  }
}
  
/**
 * Set option to vector of vector of values
 * 
 * @param name - option name
 * @param value - vector of vector of values
 */
void Options::setOpt(const std::string & name, std::vector< std::vector<std::string> > & values ) {
  ///@todo should roll into PgOptions support for this rather than using a stand alone hack
  if (!isOptDefined(name)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
  m_Options[0].mustFindOpt(name)->setValue(ToStr(values.size()));
  for (size_t i =0; i < values.size(); i++ ) {
    pushOpt(name, values[i]);
  }
}

/**
 * Add a value to a vector of values for option
 *
 * @param name - option name
 * @param value - value to push onto vector of values
 */
void Options::pushOpt( const std::string &name, const std::string & value ) {
  if (!isOptDefined(name)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
  m_Options[0].mustFindOpt(name)->pushValue(value);
}

/**
 * Add a vector of values to the values for an option
 *
 * @param name - option name
 * @param value - vector of values to push
 */
void Options::pushOpt ( const std::string & name, std::vector< std::string > values ) {
  if (!isOptDefined(name)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
  ///@todo should roll into PgOptions support for this rather than using a stand alone hack
  int nextRow = 0;
  
  while ( m_Options[0].findOpt( name + "::" + ToStr( nextRow )) != NULL ) {
    nextRow++;
  }
  m_Options[0].mustFindOpt(name)->setValue(ToStr(nextRow + 1));
  defOptMult("", name + "::" + ToStr(nextRow),PgOpt::STRING_OPT,"","");
  m_Options[0].mustFindOpt( name + "::" + ToStr(nextRow))->m_values = values;
}

/**
 * Parse and set options from argv
 *
 * @param argv - arg vector
 */
int Options::parseArgv( const char * const * const argv, int start ) {
  return m_Options[0].parseArgv( argv, start );
}

PgOpt* Options::getPgOpt(const std::string& name, int oI) {
  if (!isOptDefined(name, oI)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
  return m_Options[oI].mustFindOpt(name);
}

/**
 * Get the boolean value of an option
 *
 * @param name - the name of the option
 */
bool Options::getOptBool(const std::string& name, int oI) {
  if (!isOptDefined(name,oI)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
  if ((getOpt(name,oI) != "true") && (getOpt(name,oI) != "false") && (getOpt(name,oI) != ""))
  {
	  Err::errAbort("Option " + name + " must be set to either true or false.");
  }
  return m_Options[oI].getBool(name );
}

/**
 * Get the integer value of an option
 *
 * @param name - the name of the option
 */
int Options::getOptInt(const std::string& name, int oI) {
  if (!isOptDefined(name,oI)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
  return m_Options[oI].getInt(name);
}

/**
 * Get the integer value of an option
 *
 * @param name - the name of the option
 */
double Options::getOptDouble(const std::string& name, int oI) {
  if (!isOptDefined(name,oI)) {Err::errAbort("Option " + name + " cannot be found in the options for this engine.");}
  return m_Options[oI].getDouble(name);
}

/**
 * Return a vector of values for a particular option
 * 
 * @param name - option name
 */
std::vector< std::string >& Options::getOptVector (const std::string &name, int oI) {
    if (!isOptDefined(name,oI)) {Err::errAbort("Option " + name + " cannot be found in the options snapshot for this engine.");}
    return m_Options[oI].mustFindOpt( name )->m_values;
}

std::string Options::getXMLParameterFileName(int oI) {
    return m_Options[oI].getXMLParameterFileName();
}

std::string Options::getXMLParameterFileGuid(int oI) {
    return m_Options[oI].getXMLParameterFileGuid();
}

/**
 * Get the name of the Engine
 */
std::string Options::getProgName(int oI) {
    return m_Options[oI].getProgName();
}
  
void Options::printOptions(const std::string &prefix, int oI) {
  // Spit out to the log the options
  ///@todo kind of ugly how we are abusing PgOptions
  for(size_t i=0; i<m_Options[oI].m_option_vec.size(); i++) {
    std::string name = m_Options[oI].m_option_vec[i]->m_longName;
    std::string vals;
    if(m_Options[oI].m_option_vec[i]->m_values.size() > 0) {
      vals =  "'" + m_Options[oI].m_option_vec[i]->m_values[0] + "'";
      for(size_t j=1; j< m_Options[oI].m_option_vec[i]->m_values.size(); j++)
        vals += ", '" + m_Options[oI].m_option_vec[i]->m_values[j] + "'";
    }
    else {vals = "'" + m_Options[oI].m_option_vec[i]->getValue() + "'";}
    Verbose::out(2, prefix + "Option '" + name + "' = " + vals);
  }
}

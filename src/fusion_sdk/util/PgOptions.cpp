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
 * @file   PgOptions.cpp
 * @author Chuck Sugnet & Harley Gorrell
 * @date   Tue May  3 14:30:35 2005
 * @brief  Definitions for PgOptions class.
 */

//

#include "util/Convert.h"
#include "util/Err.h"
#include "util/PgOptions.h"
#include "util/Util.h"
//
#include <cstdio>
#include <cstdlib>
#include <iostream>
//

#ifndef APT_PGOPTIONS_NO_XERCES

#define XML_LIBRARY
#define XERCES_STATIC_LIBRARY
//#include "../../external/xerces/src/xercesc/util/PlatformUtils.hpp"
#include "../../external/xerces/src/xercesc/parsers/SAXParser.hpp"
#include "../../external/xerces/src/xercesc/util/OutOfMemoryException.hpp"
#include "../../external/xerces/src/xercesc/sax/HandlerBase.hpp"
#include "../../external/xerces/src/xercesc/util/XMLUniDefs.hpp"
#include "../../external/xerces/src/xercesc/util/XMLUni.hpp"
#include "../../external/xerces/src/xercesc/sax/AttributeList.hpp"
#include "../../external/xerces/src/xercesc/framework/XMLPScanToken.hpp"
#include "../../external/xerces/src/xercesc/util/XMLString.hpp"
#include "../../external/xerces/src/xercesc/framework/LocalFileInputSource.hpp"
#include "../../external/xerces/src/xercesc/framework/URLInputSource.hpp"
#include "util/AffxString.h"
#include "util/Fs.h"
class PgOptionsSAXHandler : public XERCES_CPP_NAMESPACE::HandlerBase
{
private:
	PgOptions* m_pPgOptions;
	std::vector<std::string>* m_pvFileNames;
    unsigned int m_uiOptionCount;
	AffxString m_strPrevCategory;
	AffxString m_strAnalysis;

public :
	PgOptionsSAXHandler(PgOptions* pPgOptions, std::vector<std::string>& vFileNames) : 
		m_pPgOptions(pPgOptions), m_pvFileNames(&vFileNames), m_uiOptionCount(0) 
	{
		if (m_pPgOptions == NULL) {Err::errAbort("PgOptionsSAXHandler must be constructed with a valid PgOptions pointer.");}
	}
	~PgOptionsSAXHandler() {}

    unsigned int getOptionCount() const {return m_uiOptionCount;}

	void warning(const XERCES_CPP_NAMESPACE::SAXParseException& exc) {Verbose::out(1, "WARNING: " + toString(exc.getMessage()));}
	void error(const XERCES_CPP_NAMESPACE::SAXParseException& exc) {Err::errAbort(toString(exc.getMessage()));}
	void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exc) {Err::errAbort(toString(exc.getMessage()));}

	void startDocument() {m_uiOptionCount = 0;}

	void endDocument()
	{
		if (m_strAnalysis != "")
		{
			Verbose::out(1, "\tanalysis = " + m_strAnalysis);
			if (m_pPgOptions->mustFindOpt("analysis")->m_allowMultiple)
			{
				m_pPgOptions->push("analysis", m_strAnalysis);
			}
			else
			{
				m_pPgOptions->set("analysis", m_strAnalysis);
			}
		}
	}

	static std::string toString(const XMLCh* const in)
	{
		char* p = XERCES_CPP_NAMESPACE::XMLString::transcode(in);
		AffxString str = p;
		XERCES_CPP_NAMESPACE::XMLString::release(&p);
		return str.trim();
	}


        void startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes)
        {
		std::string strElementName = toString(name);
//		Verbose::out(1, "Element=" + strElementName);
		m_uiOptionCount++;
		AffxString strCategoryName;
		std::string strOptionName;
		std::string strDescription;
		std::string strCurrentValue;
		std::string strGuid;
		for (unsigned int iIndex = 0; (iIndex < attributes.getLength()); iIndex++)
		{
			std::string strAttributeName = toString(attributes.getName(iIndex));
			std::string strAttributeType = toString(attributes.getType(iIndex));
			std::string strAttributeValue = toString(attributes.getValue(iIndex));
//			Verbose::out(1, "AttributeName=" + strAttributeName + ", AttributeType=" + strAttributeType + ", AttributeValue=" + strAttributeValue);
			if ((strElementName == "ParameterFile") && (strAttributeName == "guid")) 
			{
				Verbose::out(1, "\tguid = " + strAttributeValue);
				if (m_pPgOptions->getXMLParameterFileGuid() == "")
				{
					m_pPgOptions->m_strXMLParameterFileGuid = strAttributeValue; 
				}
				break;
			}			
			else if ((strElementName == "JobOrder") && (strAttributeName == "joGUID")) 
			{
				Verbose::out(1, "\tguid = " + strAttributeValue);
				if (m_pPgOptions->getXMLParameterFileGuid() == "")
				{
					m_pPgOptions->m_strXMLParameterFileGuid = strAttributeValue; 
				}
				break;
			}
			else if (strAttributeName == "analysis") {strCategoryName = strAttributeValue;}
			else if (strAttributeName == "name") {strOptionName = strAttributeValue;}
			else if (strAttributeName == "description") {strDescription = strAttributeValue;}
			else if (strAttributeName == "currentValue") {strCurrentValue = strAttributeValue;}
		}
		if (strElementName == "Parameter")
		{
			if (strCategoryName != "")
			{
				if (!m_pPgOptions->isOptDefined("analysis")) 
				{
					Verbose::out(1, "Specified option is not defined. Name: " + strOptionName);
				}
				else
				{
					if (strCategoryName != m_strPrevCategory)
					{
						if (m_strAnalysis != "")
						{
							Verbose::out(1, "\tanalysis = " + m_strAnalysis);
							if (m_pPgOptions->mustFindOpt("analysis")->m_allowMultiple)
							{
								m_pPgOptions->push("analysis", m_strAnalysis);
							}
							else
							{
								m_pPgOptions->set("analysis", m_strAnalysis);
							}
						}
						if (strCurrentValue == "")
						{
							m_strAnalysis = strOptionName;
						}
					}
					else
					{
						if (strCurrentValue != "")
						{
	//						if (::getInt(::getInt(strCurrentValue)) == strCurrentValue)
	//						{
	//							m_strAnalysis += "." + strOptionName + "=" + strCurrentValue;
	//						}
	//						else
	//						{
								m_strAnalysis += "." + strOptionName + "='" + strCurrentValue + "'";
	//						}
						}
					}
					m_strPrevCategory = strCategoryName;
				}
			}
			else
			{
				if (m_pPgOptions->isOptDefined(strOptionName))
				{
					Verbose::out(1, "\t" + strOptionName + " = " + strCurrentValue);
					if (strOptionName == "xml-file") {
                                                m_pPgOptions->setOptionsFromXMLFile(strCurrentValue, *m_pvFileNames);
                                        }
					else 
					{
						if (m_pPgOptions->mustFindOpt(strOptionName)->m_allowMultiple)
						{
							m_pPgOptions->push(strOptionName, strCurrentValue);
						}
						else
						{
							m_pPgOptions->set(strOptionName, strCurrentValue);
						}
					}
				}
				else 
				{
					Verbose::out(1, "Specified option is not defined. Name: " + strOptionName);
				}
			}
		}
	}
};

#endif

//
using namespace std;


//////////

PgOpt::PgOpt() {
  // set to known starting values.
  m_shortName="";
  m_longName="";
  m_help="";
  m_type=PgOpt::INVALID_OPT;
  m_defaultValue="";
  m_values.resize(0);
  // default is no multiples
  m_allowMultiple=0;
}

PgOpt::PgOpt(const PgOpt* orig) {
  m_shortName=    orig->m_shortName;
  m_longName=     orig->m_longName;
  m_help=         orig->m_help;
  m_type=         orig->m_type;
  m_defaultValue= orig->m_defaultValue;
  m_values=       orig->m_values;
  m_allowMultiple=orig->m_allowMultiple;
}

void PgOpt::clearValues() {
  m_values.resize(0);
}

void PgOpt::allowMutipleValues(int val) {
  m_allowMultiple=val;
}

std::string PgOpt::getDefaultValue() const {
  return m_defaultValue;
}
void PgOpt::resetToDefault() {
  clearValues();
}

bool PgOpt::isSet() {
  return (m_values.size()!=0);
}

void PgOpt::pushValue(const std::string& new_value) {
  m_values.push_back(new_value);
  //if ((m_allowMultiple==0) && (m_values.size() > 1)
}

int PgOpt::getValueCount() const {
  return (int)m_values.size();
}

std::vector<std::string> PgOpt::getValueVector() const {
  return m_values;
}

std::string PgOpt::getValue(size_t idx) const {
  // off the end.
  if (idx<0) {
    Err::errAbort("Out of bounds. (idx<0)");
  }
  // No value is set, return the defaultValue.
  if (m_values.size()==0) {
    return getDefaultValue();
  }
  if (idx<m_values.size()) {
    return m_values[idx];
  }
  // opps
  Err::errAbort("Out of bounds. (idx>size)");
  return string("internal error");
}
//
bool PgOpt::getValueBool(int idx) const {
  return Convert::toBool(getValue(idx));
}
int PgOpt::getValueInt(int idx) const {
  return Convert::toInt(getValue(idx));
}
double PgOpt::getValueDouble(int idx) const {
  return Convert::toDouble(getValue(idx));
}

void PgOpt::push_user_values_into(std::vector<std::string>& dest_vec)
{
  for (size_t i=0;i<m_values.size();i++) {
    dest_vec.push_back(m_values[i]);
  }
}

void PgOpt::dump() const
{
  printf("%-20s : ",m_longName.c_str());
  if (getValueCount()==0) {
    printf("<%s>",getValue(0).c_str());
  }
  else {
    int cnt=getValueCount();
    for (int i=0;i<cnt;i++) {
      printf("'%s'",getValue(i).c_str());
      if (i!=cnt-1) {
        printf(",");
      }
    }
  }
  printf("\n");
}

int PgOpt::checkParseIsOk(const std::string& value) const {
  bool success;

  if (m_type == INT_OPT) {
    Convert::toIntCheck(value, &success);
    if(!success) {
      Err::errAbort( "Argument for option: '" + m_longName + " (" + m_shortName + ") is not an integer: " + value);
    }
  }
  else if (m_type == DOUBLE_OPT) {
    Convert::toDoubleCheck(value, &success);
    if(!success) {
      Err::errAbort( "Argument for option: '" + m_longName + " (" + m_shortName + ") is not a double: " + value);
    }
  }
  // yes.
  return 1;
}

//////////////////////////////
//////////////////////////////

PgOptions::PgOptions() {
  clear();
}

PgOptions::~PgOptions() {
  clear();
}

void PgOptions::clear() {
  //
  m_progName = "";
  m_usageMsg = "";
  m_strXMLParameterFileName.clear();
  m_strXMLParameterFileGuid.clear();
  //
  m_argv.clear();
  m_args.clear();
  //
  for (size_t i=0;i<m_option_vec.size();i++) {
    delete m_option_vec[i];
  }
  m_option_vec.clear();
  m_option_section.clear();
  m_option_map.clear();

}

PgOptions& PgOptions::operator=(const PgOptions &options) {
  if(this == &options) return *this;

  clear();

  m_progName = options.m_progName;
  m_argv = options.m_argv;
  m_args = options.m_args;
  m_usageMsg = options.m_usageMsg;
  //
  appendOptions(options);
  //
 return *this;
}

void PgOptions::appendOptions(const PgOptions &options)
{
  // where do our options end?
  size_t initial_end=m_option_vec.size();
  // add all the new options.
  for(size_t i=0; i<options.m_option_vec.size(); i++) {
    addPgOpt(options.m_option_vec[i]);
  }
  // now add the section headings, adjusted for end.
  std::map<int,std::string>::const_iterator iter;
  for(iter = options.m_option_section.begin(); iter != options.m_option_section.end(); iter++) {
    m_option_section[initial_end+iter->first] = iter->second;
  }
}

PgOptions::PgOptions(const PgOptions &options) {
  m_progName = options.m_progName;
  m_argv = options.m_argv;
  m_args = options.m_args;
  m_usageMsg = options.m_usageMsg;
  m_strXMLParameterFileName=options.m_strXMLParameterFileName;
  m_strXMLParameterFileGuid=options.m_strXMLParameterFileGuid;

  for(size_t i=0; i<options.m_option_vec.size(); i++) {
      addPgOpt(options.m_option_vec[i]);
  }

  std::map<int,std::string>::const_iterator iter;
  for(iter = options.m_option_section.begin(); iter != options.m_option_section.end(); iter++) {
      m_option_section[iter->first] = iter->second;
  }
}

void PgOptions::setUsage(const std::string& msg) {
  m_usageMsg=msg;
}

std::string PgOptions::getUsage() {
  return m_usageMsg;
}

//////////

std::vector<std::string> PgOptions::getArgVector() {
  return m_args;
}

std::string PgOptions::getArg(int idx) {
  return m_args[idx];
}

int PgOptions::getArgCount() {
  return (int)m_args.size();
}

std::string PgOptions::getProgName() {
  return m_progName;
}

//
void PgOptions::setArgv(const char * const * const argv) {
  m_argv.clear();

  for (const char* const * arg=argv;*arg!=NULL;arg++) {
    m_argv.push_back(*arg);
  }
  //
  if (0<m_argv.size()) {
    m_progName=m_argv[0];
  }
}

std::string PgOptions::commandLine() {
  std::ostringstream stream;
  size_t m_argv_size=m_argv.size();

  if (0<m_argv_size) {
    stream << m_argv[0];
    for (size_t i=1;i<m_argv_size;i++) {
      stream << " ";
      stream << m_argv[i];
    }
  }
  return stream.str();
}

//////////

void PgOptions::clearValues() {
  for(unsigned int i = 0; i<m_option_vec.size(); i++) {
    m_option_vec[i]->clearValues();
  }
}

void PgOptions::bind(const std::string& opt_name,PgOpt* opt)
{
  // dont add the null string.
  if (opt_name!="") {
    m_option_map[opt_name]=opt;
  }
}

PgOpt* PgOptions::addPgOpt(const PgOpt* opt)
{
  // We didnt create this option so copy it.
  PgOpt* opt_copy=new PgOpt(opt);
  return addPgOpt_nocopy(opt_copy);
}

PgOpt* PgOptions::addPgOpt_nocopy(PgOpt* opt)
{
  //
  opt->checkParseIsOk(opt->getDefaultValue());
  //
  optionMapIterator_t i = m_option_map.find(opt->m_longName);
  if(i!=m_option_map.end()) {
    Err::errAbort("Option '" + opt->m_longName + "' already defined.");
  }
  if(!opt->m_shortName.empty()) {
    i = m_option_map.find(opt->m_shortName);
    if(i!=m_option_map.end()) {
      Err::errAbort("Option '" + opt->m_shortName + "' already defined.");
    }
  }
  // we will eventually free every opt in this vector.
  m_option_vec.push_back(opt);
  //
  bind(opt->m_longName,opt);
  bind(opt->m_shortName,opt);
  //
  return opt;
}

void PgOptions::defineOptionSection(const std::string &sectionName) {
  m_option_section[(int)m_option_vec.size()] = sectionName;
}

PgOpt* PgOptions::defineOption(const std::string& shortName,
                               const std::string& longName,
                               PgOpt::PgOptType_t type,
                               const std::string& help,
                               const std::string& defaultValue)
{
  PgOpt* opt=new PgOpt;
  // copy all the data
  opt->m_shortName=shortName;
  opt->m_longName=longName;
  opt->m_type=type;
  opt->m_help=help;
  opt->m_defaultValue=defaultValue;
  opt->m_values.resize(0);
  // add
  return addPgOpt_nocopy(opt);
}

PgOpt* PgOptions::defOpt(const std::string& shortName,
                         const std::string& longName,
                         PgOpt::PgOptType_t type,
                         const std::string& help,
                         const std::string& defaultValue)
{
  PgOpt* opt=defineOption(shortName,longName,type,help,defaultValue);
  return opt;
}

PgOpt* PgOptions::defOptMult(const std::string& shortName,
                             const std::string& longName,
                             PgOpt::PgOptType_t type,
                             const std::string& help,
                             const std::string& defaultValue)
{
  PgOpt* opt=defineOption(shortName,longName,type,help,defaultValue);
  opt->allowMutipleValues(1);
  return opt;
}

void PgOptions::printStringWidth(const std::string& str, int prefix,int currentPos, int maxWidth ) {
  Util::printStringWidth(cout, str, prefix, currentPos, maxWidth);
}

void PgOptions::usage(std::set<std::string> &hiddenOpts, bool printOpts) {
  PgOpt *opt = NULL;
  unsigned int maxLength = 0;
  unsigned int currentLength = 0;
  size_t i = 0;
  int extraChars = 6;

  cout << getUsage();
  cout << "\n";

  if(printOpts == true) {
    /* find the length of the longest option name. */
    for (i = 0; i<m_option_vec.size(); i++) {
      opt = m_option_vec[i];
      // check to see if we're hiding this option.
      if(hiddenOpts.find(opt->m_longName) != hiddenOpts.end()) {
        continue;
      }
      // subtract it off if there is a character.
      size_t length = opt->m_longName.size();
      if(maxLength < length)
        maxLength = (int)length;
    }
    // extraChars contains a padded space for options without a short flag,
    // three ' ' at the beginning and one ' ' at end.
    maxLength += 4 + extraChars;
    // cap it
    if(maxLength > 26) {
      maxLength = 26;
    }

    cout << "\noptions:\n";
    /* Loop through and print out the help. */
    for(i = 0; i<m_option_vec.size(); i++) {
      // Do we have a section header to print?
      if(m_option_section.find(i) != m_option_section.end())
          cout << " " << m_option_section[i] << "\n";

      // print out the option

      opt = m_option_vec[i];
      // check to see if we're hiding this option.
      if(hiddenOpts.find(opt->m_longName) != hiddenOpts.end()) {
        continue;
      }
      // we might have a short name.
      if(opt->m_shortName != "") {
        cout <<  "   -" << opt->m_shortName << ", ";
      }
      else {
        cout << "     ";
      }
      // we always have a long name.
      cout << "--" << opt->m_longName << " ";

      while(currentLength < maxLength) {
        cout.put(' ');
        currentLength++;
      }

      printStringWidth(opt->m_help + " [default '" + opt->m_defaultValue + "']",
                       maxLength, currentLength);
      cout << "\n";
    }
  }
}

int PgOptions::parseArgv(const char * const * const argv, int start) {
  assert(argv!=NULL);
  setArgv(argv);

  // Loop through and match options with arguments.
  size_t arg_idx=start;
  while (arg_idx<m_argv.size() && m_argv[arg_idx] != "--") {
    matchOneArg(&arg_idx);
  }
  // dump the values at the end.
  // dump(); // debug

  return (int)arg_idx;
}

void PgOptions::resetToDefaults() {
  for (size_t i=0;i<m_option_vec.size();i++) {
    m_option_vec[i]->resetToDefault();
  }
  m_argv.resize(0);
  m_args.resize(0);
}

void PgOptions::dump() {
  for (size_t i=0;i<m_option_vec.size();i++) {
    m_option_vec[i]->dump();
  }
  printf("Args:\n");
  for (size_t i=0;i<m_args.size();i++) {
    printf("  %3d : %s\n",(int)i,m_args[i].c_str());
  }
}

void PgOptions::matchOneArg(size_t* arg_idx)
{
  PgOpt *opt;
  std::string arg_opt;
  std::string arg_val;
  int have_arg_val=0;
  size_t idx;

  if (!(*arg_idx<m_argv.size())) {
    Err::errAbort("out of bounds");
  }

  //printf(">> %3d : %s\n",*arg_idx,m_argv[*arg_idx].c_str()); // debug
  arg_opt=m_argv[*arg_idx];
  *arg_idx+=1;

  // No leading "-"; Add it to the args vec...
  if (arg_opt[0]!='-') {
    m_args.push_back(arg_opt);
    return;
  }

  // "--" terminates options; Everything following "--" is an args.
  if (arg_opt=="--") {
    while (*arg_idx<m_argv.size()) {
      m_args.push_back(m_argv[*arg_idx]);
      *arg_idx+=1;
    }
    return;
  }

  // strip off "-" or "--"
  if (arg_opt[0]=='-') {
    if (arg_opt[1]=='-') {
      arg_opt.erase(0,2); // "--"
    }
    else {
      arg_opt.erase(0,1); // "-"
    }
  }
  else { // no "-" shouldnt happen.
    Err::errAbort("Argument does not have a starting dash: '"+arg_opt+"'");
  }

  // split on "=" for 'opt=val'...
  idx=arg_opt.find("=");
  if (idx!=std::string::npos) {
    have_arg_val=1;
    arg_val.assign(arg_opt,idx+1,arg_opt.size()-(idx+1));
    arg_opt.erase(idx); // erase "=..."
  }

  // should have something left
  if (arg_opt=="") {
    Err::errAbort("Shouldnt have a blank argument.");
  }

  // Might this be a negated arg? (like "--no-print"?)
  if (arg_opt.find("no-")==0) {
    string arg_opt_no=arg_opt;
    arg_opt_no.erase(0,3);
    opt=findOpt(arg_opt_no);
    if (opt!=NULL) {
      if (opt->m_type!=PgOpt::BOOL_OPT) {
        Err::errAbort("Cant use '--no-' with '"+arg_opt_no+"': Not a boolean option.");
      }
      opt->setValue("false");
      return;
    }
    // didnt find it, treat it as a normal option.
  }

  opt = findOpt(arg_opt);
  if(opt == NULL) {
    Err::errAbort("Don't recognize option: '" + arg_opt + "'");
  }

//  if (*arg_idx<m_argv.size()) {
//    printf("   %3d : %s\n",*arg_idx,m_argv[*arg_idx].c_str()); // debug
//  }

  if (opt->m_type == PgOpt::BOOL_OPT) {
    // --foo=bar
    if (have_arg_val==1) {
      opt->checkParseIsOk(arg_val);
      opt->setValue(arg_val);
    }
    // we might have a following "true/false"...
    // "--foo true"
    else {
      if (*arg_idx<m_argv.size()) {
        bool success;
        // printf("test: m_argv[%d] = '%s'\n",*arg_idx,m_argv[*arg_idx].c_str()); // debug
        Convert::toBoolCheck(m_argv[*arg_idx], &success);
        if (success) {
          opt->setValue(m_argv[*arg_idx]);
          *arg_idx+=1;
        }
        else {
          // didnt look like a bool, use 
			opt->setValue("true");
        }
      }
      // just "program --foo"
      else {
        opt->setValue("true");
      }
    }
    //
    return;
  }

  // Do we take the next value?
  if (have_arg_val==0) {
    if (*arg_idx<m_argv.size()) {
      arg_val=m_argv[*arg_idx];
      *arg_idx+=1;
    }
    else {
      arg_val="";
    }
  }

  // Add the value
  if (opt->checkParseIsOk(arg_val)==1) {
    if (opt->m_allowMultiple==1) {
      opt->pushValue(arg_val);
    }
    else {
	  if (opt->m_longName == "xml-file") {
		if (opt->isSet()) {Err::errAbort("The xml-file option has already been set. Only one xml-file can be specified.");}
		std::vector<std::string> vFileNames;
		setOptionsFromXMLFile(arg_val, vFileNames);
	  }
      opt->setValue(arg_val);
    }
  }
  else {
    Err::errAbort("bad parse setting '" + arg_opt +"' to '" + arg_val + "'");
  }
}

void PgOptions::clear(const std::string &name)
{
    mustFindOpt(name)->clearValues();
}

bool PgOptions::getBool(const std::string& opt_name)
{
  PgOpt *opt = mustFindOpt(opt_name);
  return opt->getValueBool(0);
}
double PgOptions::getDouble(const std::string& opt_name)
{
  PgOpt *opt = mustFindOpt(opt_name);
  return opt->getValueDouble(0);
}
int PgOptions::getInt(const std::string& opt_name)
{
  PgOpt *opt = mustFindOpt(opt_name);
  return opt->getValueInt(0);
}

//////////

int PgOptions::argc() {
  return (int)m_argv.size();
}
std::string PgOptions::argv(int idx) {
  return m_argv[idx];
}

/**
 * Load parameters from an XML file.
 * 
 * @param strFileName - The name of the XML file to load parameters from.
 */
void PgOptions::setOptionsFromXMLFile(const std::string& strFileNameIn, std::vector<std::string>& vFileNames)
{
#ifndef APT_PGOPTIONS_NO_XERCES
	AffxString strFileName = strFileNameIn;

	Verbose::out(1, "*");
	Verbose::out(1, "Loading options from file: " + strFileName);
		
	if (vFileNames.size() > 7) {Err::errAbort("Possible run away recursion situation found in PgOptions::setOptionsFromXMLFile(...)");}
	for (int iIndex = 0; (iIndex < (int)vFileNames.size()); iIndex++)
	{
		if (Fs::basename(strFileName) == vFileNames[iIndex]) {Err::errAbort("Possible run away recursion situation found in PgOptions::setOptionsFromXMLFile(...)");}
	}
	vFileNames.push_back(Fs::basename(strFileName));
	
	if (m_strXMLParameterFileName == "")
	{
		m_strXMLParameterFileName = strFileName;
	}
	// Initialize the XML4C system
	try
	{
		XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
	}

	catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch)
	{					
		Err::errAbort("PgOptions::setParametersFromXMLFile() failed at XMLPlatformUtils::Initialize(). Msg: " + PgOptionsSAXHandler::toString(toCatch.getMessage()) + " FileName: " + strFileName);
	}
	
	//
	//  Create a SAX parser object to use and create our SAX event handlers
	//  and plug them in.
	//
	XERCES_CPP_NAMESPACE::SAXParser* parser = new XERCES_CPP_NAMESPACE::SAXParser;
	PgOptionsSAXHandler handler(this, vFileNames);
	parser->setDocumentHandler(&handler);
	parser->setErrorHandler(&handler);
	parser->setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setDoSchema(false);
	parser->setValidationSchemaFullChecking(false);
	
	//
	//  Ok, lets do the progressive parse loop. On each time around the
	//  loop, we look and see if the handler has found what its looking
	//  for. When it does, we fall out then.
	//
	unsigned long duration;
    int errorCount = 0;
	try
	{
		// Create a progressive scan token
		XERCES_CPP_NAMESPACE::XMLPScanToken token;
		const unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		try
		{
			XMLCh* p = XERCES_CPP_NAMESPACE::XMLString::transcode(strFileName.c_str());
			try
			{
				XERCES_CPP_NAMESPACE::URLInputSource source(p);
				if (!parser->parseFirst(source, token))
				{
					Err::errAbort("PgOptions::setParametersFromXMLFile() Cannot open or parse xml-file. FileName: " + strFileName);
				}
			}
			catch(...)
			{				
				XERCES_CPP_NAMESPACE::LocalFileInputSource source(p);
				if (!parser->parseFirst(source, token))
				{
					Err::errAbort("PgOptions::setParametersFromXMLFile() Cannot open or parse xml-file. FileName: " + strFileName);
				}
			}
			XERCES_CPP_NAMESPACE::XMLString::release(&p);
			//
			//  We started ok, so lets call scanNext() until we find what we want
			//  or hit the end.
			//
			bool gotMore = true;
			while (gotMore && !parser->getErrorCount())
			{
				gotMore = parser->parseNext(token);
			}

			const unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			duration = endMillis - startMillis;

			errorCount = parser->getErrorCount();
			//
			//  Reset the parser-> In this simple progrma, since we just exit
			//  now, its not technically required. But, in programs which
			//  would remain open, you should reset after a progressive parse
			//  in case you broke out before the end of the file. This insures
			//  that all opened files, sockets, etc... are closed.
			//
			parser->parseReset(token);
		}
		catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch)
		{
			Err::errAbort("PgOptions::setParametersFromXMLFile() failed with an XMLException. Msg: " + PgOptionsSAXHandler::toString(toCatch.getMessage()) + " FileName: " + strFileName);
		} 
		catch(...) {Err::errAbort("PgOptions::setParametersFromXMLFile() Exception thrown while parsing xml-file. FileName: " + strFileName);}
	}
	catch (const XERCES_CPP_NAMESPACE::OutOfMemoryException&)
	{		
		delete parser;
		XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();			
		Err::errAbort("PgOptions::setParametersFromXMLFile() failed with an OutOfMemoryException. FileName: " + strFileName);
	}
	catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch)
	{
		delete parser;
		XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
		Err::errAbort("PgOptions::setParametersFromXMLFile() failed with an XMLException. Msg: " + PgOptionsSAXHandler::toString(toCatch.getMessage()) + " FileName: " + strFileName);
	}
	
//	Verbose::out(1, "XMLFileName = " + strFileName + ", OptionCount = " + ToStr(handler.getOptionCount()));
	Verbose::out(1, "*");
	delete parser;
	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
	vFileNames.pop_back();
#endif
}



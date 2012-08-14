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
 * @file   PgOptions.h
 * @author Chuck Sugnet && harley
 * @date   Tue May  3 11:49:37 2005
 * @brief  Classes for reading program command line options.
 */

/*
  @page PgOptions MANUAL: PgOptions (NON-OFFICIAL-RELEASE)

  @section PgOptionsContents Contents

  @section PgOptionsIntro Introduction

  PgOptions is the APT library for working with command line
  options.  With this library, a program can declare the
  list of options it supports along with their kind and
  defaults.  PgOptions will process the command line and
  then the program can query what was set.

  Like standard unix programs, an option of "--" ends option
  parsing.  All the remaning arguments are args to the
  program.

  There are three kinds of options: BOOL, DOUBLE and STRING.
  All three kinds may have a value appended to the option name with "=".
  (Like: "--double-example=10.5", "--output-dir=foo".)

  BOOL values may have a prefix of "-no-" or "--no-" to set
  the value to "false".  If the arg after a bool option is
  "true/1" or "false/0" it is used as the value.  Otherwise
  the option is set to true.  (Note that this might make
  working with a file called "true" ambiguous.  In that case
  use "--" to seperate the args from the option.)

  Non-option args to the program are put into the "args"
  list which the program can retreive with "getArg(idx)".

  @section PgOptionsExample Example

  Normal usage looks like:

@verbatim
int main (int argc,char* argv[]) {

  PgOptions opts;
  opts->setUsage("A nice paragraph about what this program does.");
  opts->defineOption("p","print",PgOpt::BOOL_OPT,
                     "Do you want to print the output?",
                     "true");

  opts->parseArgv(argv);

  if (opts->getBool("print")) {
    doPrint();
  }
}
@endverbatim

  @section PgOptionsNotes Notes

  * Please try and write the options in the "positive sense".
  If the default is to print, then define the option "print"
  to be true.  The user can use "--no-print" when they dont
  want to.  (Dont define "--no-print".)

 */

#ifndef PGOPTIONS_H
#define PGOPTIONS_H

// "APT_PGOPTIONS_NO_XERCES" is used to remove the use of
// Xerces XML functions in PgOptions.
// affy/exact uses PgOptions, but has no need of XML.
// providing an option to disable

//
#include "portability/apt-win-dll.h"
#include "util/Err.h"
//
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

/// @class PgOpt
/// @brief The definition of this option and the values.
class APTLIB_API PgOpt {
public:
  /** Types of allowed options. */
  typedef enum PgOptType {
    INVALID_OPT,
    BOOL_OPT,
    DOUBLE_OPT,
    INT_OPT,
    STRING_OPT,
  } PgOptType_t;

public:
  std::string m_shortName;           ///< Short name without the '-' (-h -> h).
  std::string m_longName;            ///< Long name without the '--' prefix.
  //
  std::string m_help;                ///< Maybe help, possibly NULL.
  PgOptType   m_type;                ///< Type of option.
  std::string m_defaultValue;        ///< Default value if any.
  std::vector<std::string> m_values; ///< Values for this option.
  int         m_allowMultiple;       ///< Allow mutiple args
  //int         m_numargs;

  /// @brief     Creates and inits a new PgOpt
  PgOpt();
  PgOpt(const PgOpt* orig);

  /// @brief     Allows mutiple values for this option?
  /// @param     val       1=yes 0=no
  void allowMutipleValues(int val);
  /// @brief     was this option set by the user?
  /// @return    true if it was set, false if it is the default value.
  bool isSet();
  /// @brief     The number of values for this option.
  /// @return    int
  int getValueCount() const;
  /// @brief     Get the string value of this option
  /// @param     idx       (optional) the index
  /// @return    string value
  std::string getValue(size_t idx=0) const;
  /// @brief     Get the value as a boolean.  Abort if it cant be converted.
  /// @param     idx       (optional) the index
  /// @return    boolean
  bool   getValueBool(int idx=0) const;
  /// @brief     Get the value as an int. Abort if it cant be converted.
  /// @param     idx       (optional) the index
  /// @return    the value as an int
  int    getValueInt(int idx=0) const;
  /// @brief     Get the value as a double. Abort if it cant be converted.
  /// @param     idx       (optional) the index
  /// @return    the value as a double
  double getValueDouble(int idx=0) const;
  /// @brief     The default value of this option
  /// @return    string
  std::string getDefaultValue() const;

  /// @brief     Clear all the values on this option
  void clearValues();
  /// @brief     Set the value of this option to a single value.
  /// @param     new_value
  void setValue(const std::string &new_value) {
    m_values.resize(0);
    pushValue(new_value);
  }
  /// @brief     Push the new value onto the list of values for this option.
  /// @param     new_value
  /// @remarks   only valid for options which allow mutiple values.
  void pushValue(const std::string& new_value);
  /// @brief     same as clearValues()
  void resetToDefault();

  std::vector<std::string> getValueVector() const;

  /// @brief     Check to see if the parse of the string is ok for this option.
  /// @param     value     string to parse.
  /// @return    1 if ok.
  int checkParseIsOk(const std::string& value) const;

  /// @brief     Push all our user supplied values into another vector.
  /// @param     dest_vec   vector to add them to.
  void push_user_values_into(std::vector<std::string>& dest_vec);

  /// @brief     Dump the internal state of the option for debugging.
  void dump() const;
};


//////////
//////////

#ifndef APT_PGOPTIONS_NO_XERCES
class PgOptionsSAXHandler;
#endif

/// @brief
class APTLIB_API PgOptions {

#ifndef APT_PGOPTIONS_NO_XERCES
	friend class PgOptionsSAXHandler;
#endif

public:
  typedef std::map<std::string, PgOpt *> optionMap_t;
  typedef optionMap_t::iterator optionMapIterator_t;

public:
  ///< Helpful message explaining the program and purpose.
  std::string m_usageMsg;

  ///< Vec of all the options defined.
  std::vector<PgOpt*> m_option_vec;
  ///< Map of indexes for option section starts
  std::map<int,std::string> m_option_section;
  ///< Map option names to PgOpt
  optionMap_t m_option_map;

  ///< name of this program.
  std::string m_progName;
  ///< A copy argv we were given.
  std::vector<std::string> m_argv;
  ///< Non option arguments.
  std::vector<std::string> m_args;

  // XML Parameter file name
  std::string m_strXMLParameterFileName;

  // XML Parameter file guid
  std::string m_strXMLParameterFileGuid;

public:

  /// @brief     Creates a new PgOptions object
  PgOptions();
  /// @brief     Creates a new PgOptions from an existing one.
  PgOptions(const PgOptions &options);

  /// @brief     Destroys a PgOptions object and its PgOpts.
  ~PgOptions();

  /// @brief clear all the data & free memory.
  void clear();
  /// @brief appendOptions
  void appendOptions(const PgOptions& options);

  /// @brief Assignment
  PgOptions& operator=(const PgOptions &options);

  /// @brief     Get the usage info
  /// @return    string
  std::string getUsage();
  /// @brief     set the usage info for this program
  /// @param     usage
  void setUsage(const std::string& usage);


  /// @brief     define an option to be handled by this option parser.
  /// @param     shortName    one or two letter option name.
  /// @param     longName     full length option name.
  /// @param     type         the type of option (string,int,bool,double)
  /// @param     help         help text.
  /// @param     defaultVal Default value to use if not set.
  /// @return    pointer to the PgOpt created.
  /// @remarks   This calls addPgOpt to add the created option to the option state.
  PgOpt* defineOption(const std::string& shortName,
                      const std::string& longName,
                      PgOpt::PgOptType_t type,
                      const std::string& help,
                      const std::string& defaultVal);

  /// @brief     Short form of defineOption
  /// @param     shortName
  /// @param     longName
  /// @param     type
  /// @param     help
  /// @param     defaultVal
  /// @return
  PgOpt* defOpt(const std::string& shortName,
                const std::string& longName,
                PgOpt::PgOptType_t type,
                const std::string& help,
                const std::string& defaultVal);
  /// @brief     Short for to define an option with multiple values.
  /// @param     shortName
  /// @param     longName
  /// @param     type
  /// @param     help
  /// @param     defaultVal
  /// @return
  PgOpt* defOptMult(const std::string& shortName,
                    const std::string& longName,
                    PgOpt::PgOptType_t type,
                    const std::string& help,
                    const std::string& defaultVal);

  /// @brief     Define a new section for options
  /// @param     sectionName name for the section
  /// @return    void
  void defineOptionSection(const std::string &sectionName);
  /// @brief     Copies and adds an PgOpt to the internal state.
  /// @param     option    option to add
  /// @return    pointer to copy.
  PgOpt* addPgOpt(const PgOpt *option);
  /// @brief     Adds an PgOpt to the internal state.
  /// @param     option    option to add
  /// @return    pointer to the passed in option
  PgOpt* addPgOpt_nocopy(PgOpt *option);

  /// @brief     Add the option name and PgOpt to our map of name,value pairs
  /// @param     opt_name  name to bind.  null strings ("") are ignored.
  /// @param     opt       option for this name.
  void bind(const std::string& opt_name,PgOpt* opt);

  /// @brief     Finds the option which is bound to this name.
  /// @param     name
  /// @return    pointer to option or NULL
  PgOpt* findOpt(const std::string &longName)
  {
    optionMapIterator_t i = m_option_map.find(longName);
    if (i == m_option_map.end()) {
        return NULL;
    }
    return i->second;
  }
  /// @brief     Same as findOpt, but aborts if not option is not found.
  /// @param     name
  /// @return    pointer to option
  PgOpt* mustFindOpt(const std::string &name)
  {
    PgOpt *opt = findOpt(name);
    if(opt == NULL) {
        Err::errAbort("Don't recognize option with name: '" + name + "'.");
    }
    return opt;
  }

  /// @brief Print a string wrapping at max width from the current position.
  /// @param str - The cstring to be printed.
  /// @param prefix - How many spaces to put on begining of newline.
  /// @param maxWidth - Where to wrap text at.
  /// @param currentPos - What position in the line is cursor currently at.
  static void printStringWidth(const std::string& str, int prefix=0,int currentPos=0, int maxWidth=70);

  /// @brief Print out a litte ditty about program and its usage.
  /// @param printOpts - Print out options and help for each one?
  void usage(bool printOpts = true) {
    std::set<std::string> toHide;
    usage(toHide, printOpts);
  }

  /// Print out a litte ditty about program and its usage.
  /// @param hiddenOpts - Set containing the long name of options to hide.
  /// @param printOpts - Print out options and help for each one?
  void usage(std::set<std::string> &hiddenOpts, bool printOpts = true);

  /// @brief Match the command line arguments from
  /// @param argv Arguments supplied to program.  argv[0] is the program name.
  /// @param start The position to start parsing at (default 1)
  /// @return the last position in argv parsed (will stop at "--")
 int parseArgv(const char * const * const argv, int start = 1);
  /// @brief the args to m_arg
  /// @param argv Arguments to copy
  void setArgv(const char * const * const argv);

  /// @brief     Match the arg found at argv[arg_idx].
  /// @param     arg_idx   arg_idx is updated to point to the next arg to match.
  void matchOneArg(size_t* arg_idx);
  /// @brief     Clear the values for an option
  /// @param     name       the name of the option
  void clear(const std::string &name);
  /// @brief     Set the value of an option
  /// @param     name       the name of the option
  /// @param     new_value  the value of the option
  void set(const std::string &name, const std::string &new_value) {
      if ((isOptDefined("xml-file")) && (name == "xml-file"))
      {
        if (mustFindOpt(name)->isSet()) {
          Err::errAbort("The xml-file option has already been set. Only one xml-file can be specified.");
        }
        std::vector<std::string> vFileNames;
        setOptionsFromXMLFile(new_value, vFileNames);
      }
      mustFindOpt(name)->setValue(new_value);
  }
  /// @brief     Add a value to an option vector
  /// @param     name       the name of the option
  /// @param     value      the value to push
  void push(const std::string &name, const std::string &value) {
    mustFindOpt(name)->pushValue(value);
  }
  /// @brief     Get the value of the option as a string.
  /// @param     opt_name  option name (short or long)
  /// @return    the value.  Abort if not found.
  std::string get(const std::string& opt_name)
  {
    PgOpt *opt = mustFindOpt(opt_name);
    return opt->getValue(0);
  }
  /// @brief     Get the value of the option as a boolean.
  /// @param     opt_name  option name (short or long)
  /// @return    the value.  Abort if not found.
  bool getBool(const std::string& opt_name);
  /// @brief     Get the value of the option as a double.
  /// @param     opt_name  option name (short or long)
  /// @return    the value.  Abort if not found.
  double getDouble(const std::string& opt_name);
  /// @brief     Get the value of the option as a int.
  /// @param     opt_name  option name (short or long)
  /// @return    the value.  Abort if not found.
  int getInt(const std::string& opt_name);

  /// @brief     The number of left over args
  /// @return    int
  int getArgCount();

  /// @brief     Get the IDXth arg.
  /// @param     index     index of arg to get.
  /// @return    string
  std::string getArg(int index);

  /// @brief     Get the args as a vector.
  /// @return    vector of args.
  std::vector<std::string> getArgVector();

  /// @brief     The name of the program (argv[0])
  /// @return    string
  std::string getProgName();
  /// @brief     same as clearValues
  void resetToDefaults();
  /// @brief     clear all the values from the PgOpts and internal state.
  void clearValues();

  /// @brief     A space seperated list of the argv we were given.
  /// @return    string
  std::string commandLine();

  /// @brief     The size of the argv we were inited with.
  /// @return    the size of argv
  int argc();

  /// @brief     Get the idxth entry in the argv which was supplied.
  /// @param     idx       the index.
  /// @return    the entry.
  std::string argv(int idx);

  /// @brief     Dump the internal state for debugging.
  void dump();

  bool isOptDefined(const std::string& name) {return (findOpt(name) != NULL);}

  std::string getXMLParameterFileName() {return m_strXMLParameterFileName;}
  std::string getXMLParameterFileGuid() {return m_strXMLParameterFileGuid;}

protected:
  void setOptionsFromXMLFile(const std::string& strFileName, std::vector<std::string>& vFileNames);
};

#endif /* PGOPTIONS_H */

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
 * @file   PgOptions.h
 * @author Chuck Sugnet
 * @date   Tue May  3 11:49:37 2005
 *
 * @brief  Classes for reading program command line options.
 *
 */
#ifndef PGOPTIONS_H
#define PGOPTIONS_H

#include <map>
#include <set>
#include <string>
#include <vector>

/**
 *  PgOptions
 * @brief Holds all command line arguments, parses options and
 *    keeps arguments.
 *
 *   Classes to make parsing of command line arguments easy and assist
 *   with usage statment if desired. PgOptions will make available the
 *   command line arguments of interest via map of option name to
 *   value specified (or default value). This doesn't conform exactly
 *   to the usual getopts type of command line options.
 *
 *   Conventions:
 *   - Short version of an option begin with '-' and is separated from the value
 *     specified (if any) by white space.
 *   - Long version of an option begins with '--' and is separated from the value
 *     specified (if any) by white space
 *
 *   Idea is to create static PgOpt classes in the source code and then
 *   use them to create a PgOptions class and parse the command line arguments.
 *   For example:
 *   <code><pre>
 *   #include "PgOptions.h"
 *   static PgOptions::PgOpt INTEGER_OPT = {
 *    "i", "integer-opt", PgOptions::INT_OPT,
 *    "Tests to see what an integer value looks like. The description of this option should wrap.",
 *    "10", NULL};
 *
 *   static PgOptions::PgOpt FLOAT_OPT = {
 *    "f", "float-opt", PgOptions::FLOAT_OPT,
 *    "Tests to see what an float value looks like.",
 *    "10.5", NULL};
 *
 *   static PgOptions::PgOpt STRING_OPT = {
 *    "s", "string-opt", PgOptions::STRING_OPT,
 *    "Tests to see what an string value looks like.",
 *    "stringOption", NULL};
 *
 *   static PgOptions::PgOpt STRING_OPT2 = {
 *    "s2", "string-opt2-really-long", PgOptions::STRING_OPT,
 *    "Tests to see what an string value looks like.",
 *    "stringOptionDefault", NULL};
 *
 *   static PgOptions::PgOpt BOOL_OPT = {
 *    "b", "bool-opt", PgOptions::BOOL_OPT,
 *    "Tests to see what an bool value looks like.",
 *    "1", NULL};
 *
 *   static PgOptions::PgOpt *optionsDef[] = {
 *    &BOOL_OPT,
 *    &INTEGER_OPT,
 *    &FLOAT_OPT,
 *    &STRING_OPT,
 *    &STRING_OPT2,
 *    NULL};  // <--- *Note* NULL terminated
 *
 *   int main(int argc, const char * const argv[]) {
 *    PgOptions *opts = NULL;
 *    string usage(
 *      "This is my program. Here is where I would give you a brief\n"
 *      "description of what it does and how it works. In an ideal world this\n"
 *      "text would all wrap nicely at 70 characters or so.\n"
 *      "\nusage:\n  "
 *      "   myProgram arg1 arg2");
 *    // Create an options class with our statically derived options before.
 *    opts = new PgOptions(usage, optionsDef);
 *    // Parse the options from the command line.
 *    opts->parseOptions(argc, argv);
 *
 *    // later on...
 *    bool myBoolean = opts->boolOpt("b");
 *    int myInt =  opts->intOpt("integer-opt");
 *    // arguments that don't go with options are accesible by getArg()
 *    char *arg = opts->getArg(0);
 *    }
 * </pre></code>
 */
class PgOptions {

public:

  /** Types of option arguments expected. */
  typedef enum PgOptType {
    BOOL_OPT,
    INT_OPT,
    FLOAT_OPT,
    STRING_OPT,  } PgOptType;

  /**
   *  PgOpt
   * @brief Individual option that can be specifed on command line.
   */
  class PgOpt {

  public:

    /// No constructor otherwise can't do static initialization...
    /// order is important.
    const char *shortName;     ///< Short name without the '-' (-h -> h).
    const char *longName;      ///< Long name without the '--' prefix.
    PgOptType   type;          ///< Type of option.
    const char *help;          ///< Maybe help, possibly NULL.
    const char *m_defaultVal;  ///< Default value if any.
    const char *m_value;       ///< Value supplied if any, NULL otherwise.
    PgOpt *next;               ///< If muliple options, duplicates are here.
    char* m_valueMalloced;     ///< pointer to malloced data for later freeing.

  public:
    /**
     * @brief Get the value of this option.
     */
    char const * const getValue() const { return m_value; }

    /**
     * @brief Get the default value of this option.
     */
    char const * const getDefaultVal() const { return m_defaultVal; }
    //
    void clearValue();
    void setValue(char const * const newValue);
    void setValueNoCopy(char const * const newValue);
    void resetToDefault();
  };

public:

  /**
   * Destructor.
   */
  ~PgOptions() {
    // also frees the memory allocated.
    resetToDefaults();
    //
    for(unsigned int i=0; i < m_ToFree.size(); i++)
      delete m_ToFree[i];
  }

  /**
   * Get the pointer for an options by looking up its name.
   * return NULL if not found.
   * @return NULL if not found PgOpt pointer otherwise.
   */
  PgOpt* findPgOpt(const std::string &name);


  /**
   * Get the pointer for an options by looking up its name.
   * Call PgAbort if not found.
   *
   * @return NULL if not found PgOpt pointer otherwise.
   */
  PgOpt* mustFindPgOpt(const std::string &name);

   /**
    * Print a string wrapping at max width from the current
    * position.
    * @param str - The cstring to be printed.
    * @param prefix - How many spaces to put on begining of newline.
    * @param maxWidth - Where to wrap text at.
    * @param currentPos - What position in the line is
    *                      cursor currently at.
    */
  static void printStringWidth(const char *str, int prefix=0,
                        int currentPos=0, int maxWidth=70 );

  /**
   * Print out a litte ditty about program and its usage.
   * @param printOpts - Print out options and help for each one?
   */
  void usage(bool printOpts = true) {
    std::set<std::string> toHide;
    usage(toHide, printOpts);
  }

  /**
   * Print out a litte ditty about program and its usage.
   * @param hiddenOpts - Set containing the long name of options to hide.
   * @param printOpts - Print out options and help for each one?
   */
  void usage(std::set<std::string> &hiddenOpts, bool printOpts = true);


  /**
   * Look up the boolean value of an option.
   * @param name - Long name of option.
   * @return true if option set to true false otherwise.
   */
  bool boolOpt(const char* name);
  bool boolOpt(PgOptions::PgOpt& pgopt) {
    return boolOpt(pgopt.longName);
  }

  /**
   * Look up an option and give integer value for it.
   * @param name - Long name of option.
   * @return int - value of option.
   */
  int intOpt(const char* name);
  int intOpt(PgOptions::PgOpt& pgopt) {
    return intOpt(pgopt.longName);
  }

  /**
   * Look up an option and give float value for it.
   * @param name - Long name of option.
   * @return float - value of option.
   */
  float floatOpt(const char* name);
  float floatOpt(PgOptions::PgOpt& pgopt) {
    return floatOpt(pgopt.longName);
  }

  /**
   * Look up the string value of an option.
   * @param name - Long name of option.
   * @return string value assocated with option.
   */
  const char* strOpt(const char *name);
  const char* strOpt(PgOptions::PgOpt& pgopt) {
    return strOpt(pgopt.longName);
  }

  /**
   * Match the command line arguments from
   * @param argc Number of arguments.
   * @param argv Arguments supplied to program.
   */
  void parseOptions(int argc, const char *const argv[]);

  /**
   * Constructor Use individual PtOpts to populate create a PgOptions class.
   * Then match up the arguments to options that are possible using
   * parseOptions().
   *
   * @param usage - Message to the user printed in usage() call.
   * @param options - Array of valid options program can be called
   *                 with.
   * @param allowDupes - Is it ok to have duplicates of the same option?
   */
  PgOptions(const std::string &usage, PgOpt *options[] = NULL, bool allowDupes = true);

  /**
   * Get the number of arguments that weren't used matched to
   * a particular option. For example: <br>
   * <code>
   * >myProg -optInt 1 -optFloat 2.0 file1 file2 <br>
   * getNumArgs() == 2; // file1 & file2         <br>
   * </code>
   * @return Number of arguments that weren't matched to options.
   */
  unsigned int getNumArgs() { return args.size(); }

  /**
   * @param index - Number of command line argument desired after
   *   the option matching occurs.
   * @return character string argument supplied to program.
   */
  const char *getArg(int index) { return args[index]; }

  /**
   * Get the program name (i.e. whatever was in argv[0])
   * @return - program name.
   */
  std::string getProgName() { return progName; }

  /**
   * Reset all the options to their default value.
   */
  void resetToDefaults();

  /**
   * PgOptions gets its kickers in a bind when it sees
   * the options alread have values.
   * This function *ahem* just sets the values to NULL.
   * Thats why it has the "hack_" prefix.
   */
  static void hack_resetOptionsList(PgOptions::PgOpt **optList);

public:
  typedef std::map<std::string, PgOpt *> optionMap_t;
  typedef optionMap_t::iterator optionMapIterator_t;

private:
  ///< Map to look up options by long or short name.
  optionMap_t optionMap;

private:

  ///< Original option specification for program.
  PgOpt **optSpec;
  ///< Helpful message explaining program name and purpose.
  std::string usageMsg;
  ///< Program Name.
  std::string progName;
  ///< Non option arguments to program.
  std::vector<const char*> args;
  ///< Memory to be freed.
  std::vector<PgOpt *> m_ToFree;
  ///< Should duplicates of the same argument be allowed?
  bool m_AllowDupes;

  /**
   * Add the option to the map maintained by PgOption after doing some error
   * checking
   *
   * @param option - Option to be added to PgOptions possibilities.
   */
  void addOnePgOpt(PgOpt *option);

  /**
   * Check the value to make sure it is consistent with requested type of option.
   *
   * @param opt - Option to check.
   * @param value - Argument supplied on command line.
   */
  void checkOptType(PgOpt *opt, const char *value);

  /**
   * Make sure option requested is valid and type supplied (if any) matches data
   * supplied.
   *
   * @param name - Name of option to match (with or without '-'s on it);
   * @param argv - Arguments left to be parsed. argv[0] == s.
   * @param argIx - Argument index, increment as more command line
   *     arguments are used.
   */
  void matchOption(const char *name, const char * const argv[], int *argIx);

  /**
   * Return a pointer to the next character that is white space
   * or NULL if none found.
   * @param s - cstring to find white space in.
   * @return - Pointer to next whitespace character or NULL if none
   *   found.
   */
  static const char *nextWhiteSpace(const char *s);

};

#endif /* PGOPTIONS_H */

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
 * @file   PgOptions.cpp
 * @author Chuck Sugnet
 * @date   Tue May  3 14:30:35 2005
 *
 * @brief  Definitions for PgOptions class.
 *
 */

//
#include "PgOptions.h"
//
#include <iostream>
#include <stdio.h>
//
#include "Convert.h"
#include "Err.h"
#include "Util.h"

//
using namespace std;

/**
 * @brief Clear the value of this option, freeing memory if needed.
 */
void PgOptions::PgOpt::clearValue()
{
  // free any malloced memory.
  if (m_valueMalloced!=NULL) {
    free(m_valueMalloced);
    m_valueMalloced=NULL;
  }
  m_value=NULL;
}

/**
 * @brief Reset this option to its default value.
 */
void PgOptions::PgOpt::resetToDefault() {
  clearValue();
  // no need to copy,
  setValue(m_defaultVal);
}

/**
 * @brief  Set the string value but dont copy it. (Dont free it either.)
 */
void PgOptions::PgOpt::setValueNoCopy(char const * const newValue) {
  clearValue();
  m_value=newValue;
}

/**
 * @brief   Set the string value of the option.
 * @param   newValue the new string value.  (Duplicated if required.)
 * @remarks Note that we dont duplicate data we know to be static.
 */
void PgOptions::PgOpt::setValue(char const * const newValue) {
  clearValue();
  // no new value
  if (newValue==NULL) {
    m_value=NULL;
  }
  // no need to duplicate static data...
  else if (newValue==m_defaultVal) {
    // ... just copy the pointer.
    m_value=m_defaultVal;
  }
  // ...duplicate the new value.
  else {
    m_valueMalloced=strdup(newValue);
    assert(m_valueMalloced!=NULL);
    m_value=m_valueMalloced;
  }
}

/**
 * Get the pointer for an options by looking up its name.
 * return NULL if not found.
 *
 * @return NULL if not found PgOpt pointer otherwise.
 */
PgOptions::PgOpt* PgOptions::findPgOpt(const std::string &longName) {
  map<string, PgOptions::PgOpt *>::iterator i = optionMap.find(longName);
  if(i == optionMap.end())
    return NULL;
  return i->second;
}

/**
 * Get the pointer for an options by looking up its name.
 * Call errAbort if not found.
 *
 * @return NULL if not found PgOpt pointer otherwise.
 */
PgOptions::PgOpt* PgOptions::mustFindPgOpt(const std::string &name) {
  PgOpt *opt = findPgOpt(name);
  if(opt == NULL)
    Err::errAbort("Don't recognize option with name: '" +
            string(name) + "'.");
  return opt;
}

/**
 * Print a string wrapping at max width from the current
 * position.
 * @param str - The cstring to be printed.
 * @param prefix - How many spaces to put on begining of newline.
 * @param maxWidth - Where to wrap text at.
 * @param currentPos - What position in the line is
 *                      cursor currently at.
 */
void PgOptions::printStringWidth(const char *str, int prefix,
				 int currentPos, int maxWidth ) {
  Util::printStringWidth(cout, str, prefix, currentPos, maxWidth);
}

/**
 * Print out a litte ditty about program and its usage.
 * @param hiddenOpts - Set containing the long name of options to hide.
 * @param printOpts - Print out options and help for each one?
 */
void PgOptions::usage(std::set<std::string> &hiddenOpts, bool printOpts) {
  PgOpt *opt = NULL;
  unsigned int maxLength = 0;
  unsigned int currentLength = 0;
  int i = 0;
  int extraChars = 6;

  cout << usageMsg.c_str();
  cout.put('\n');
  if(printOpts == true) {
    fprintf(stdout, "\noptions:\n");
    /* find the length of the longest option name. */
    for(i = 0; optSpec[i] != NULL; i++) {
      opt = optSpec[i];
      // check to see if we're hiding this option.
      if(hiddenOpts.find(opt->longName) != hiddenOpts.end())
        continue;
      // extraChars contains a padded space for options without a short flag,
      // subtract it off if there is a character.
      unsigned int length = strlen(opt->longName) + extraChars;
      if(maxLength < length)
	maxLength = length;
    }
    maxLength += 4; // three ' ' at the beginning and one ' ' at end.
    if(maxLength > 26)
      maxLength = 26;

    /* Loop through and print out the help. */
    for(i = 0; optSpec[i] != NULL; i++) {
      opt = optSpec[i];
      // check to see if we're hiding this option.
      if(hiddenOpts.find(opt->longName) != hiddenOpts.end())
        continue;
      if(strlen(opt->shortName) > 0)
	cout <<  "   -" << opt->shortName << ", --" << opt->longName << " ";
      /* Treat the longName as the shortName if the shortName is null. */
      else
	/* Line up just the long name if no short name. */
	cout <<  "       --" << opt->longName << " ";
      currentLength = strlen(opt->longName) + 10;

      while(currentLength < maxLength) {
	cout.put(' ');
	currentLength++;
      }

      printStringWidth(opt->help, maxLength, currentLength);
      cout.put('\n');
    }
  }
}

/**
 * Add the option to the map maintained by PgOption after doing some error
 * checking
 *
 * @param option - Option to be added to PgOptions possibilities.
 */
void PgOptions::addOnePgOpt(PgOpt *option) {

  string longName(option->longName);
  string shortName(option->shortName);
  optionMapIterator_t iter;

  if(option->getValue() != NULL) {
      Err::errAbort("Specify defaults as 'defaults', not initial values."
            "(see: '" + ToStr(option->longName) + "'='" +
            ToStr(option->getValue())+"')");
  }

  /* If option has been seen add to end of list. */
  iter = optionMap.find(longName);
  if(iter != optionMap.end()) {
    Err::errAbort("Option '" + longName + "' already defined.");
  }
  else { /* new option. */
    optionMap[longName] = option;
    /* ignore a null shortName */
    if (shortName != "")
      optionMap[shortName] = option;
  }
}

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
PgOptions::PgOptions(const std::string &usage, PgOpt *options[], bool allowDupes) {
  m_AllowDupes = allowDupes;
  usageMsg = usage;
  int optIx = 0;
  optSpec = options;
  for(optIx = 0; (options != NULL) && (options[optIx] != NULL); optIx++) {
    if(options[optIx]->getDefaultVal() == NULL) {
        Err::errAbort("Please specify default value for option '" +
              string(options[optIx]->longName) + "'");
    }
    checkOptType(options[optIx], options[optIx]->getDefaultVal());
    addOnePgOpt(options[optIx]);
  }
}

/**
 * Check the value to make sure it is consistent with requested type of option.
 *
 * @param opt - Option to check.
 * @param value - Argument supplied on command line.
 */
void PgOptions::checkOptType(PgOpt *opt, const char *value) {
  bool success = true;

  if(value == NULL) {
    Err::errAbort("expecting a value to be associated with option: '" + ToStr(opt->longName) + "'");
  }
  if(opt->type == INT_OPT) {
    Convert::toIntCheck(value, &success);
    if(!success) {
      Err::errAbort( "Argument for option: '" + string(opt->longName) +
                       " (" + string(opt->shortName) + ") is not an integer: " +
                       value);
    }
  }
  else if(opt->type == FLOAT_OPT) {
    Convert::toFloatCheck(value, &success);
    if(!success) {
      Err::errAbort( "Argument for option: '" + string(opt->longName) +
                       " (" + string(opt->shortName) + ") is not an float: " +
                       value);
    }
  }
}

/**
 * Make sure option requested is valid and type supplied (if any) matches data
 * supplied.
 *
 * @param name - Name of option to match (with or without '-'s on it);
 * @param argv - Arguments left to be parsed. argv[0] == s.
 * @param argIx - Argument index, increment as more command line
 *     arguments are used.
 */
void PgOptions::matchOption(const char *name, const char * const argv[], int *argIx) {
  PgOpt *opt = NULL;
  const char *s = name;

  while(s[0] == '-') // strip off any leading '-'s
    s++;

  opt = findPgOpt(string(s));
  if(opt == NULL)
    Err::errAbort("Don't recognize option: '" + string(name) + "'");
  /* Have we already seen this option? if so put it at the end of the list. */
  if(opt->getValue() != NULL) {
    if(!m_AllowDupes) {
      Err::errAbort("Option: " + ToStr(s) + " has already been specified with value: '" +
                    ToStr(opt->getValue()) + "'.");
    }
    PgOpt *p = new PgOpt, *tail = opt;
    /* Copy parent data. */
    p->shortName = opt->shortName;
    p->longName = opt->longName;
    p->type = opt->type;
    p->help = opt->help;
    p->m_defaultVal = opt->m_defaultVal;
    p->m_valueMalloced=NULL;
    p->setValue(NULL);
    p->next = NULL;

    /* Find last argument. */
    while(tail->next != NULL) {
      tail = tail->next;
    }
    tail->next = p;
    checkOptType(opt, argv[1]);
    //p->value = argv[1];
    p->setValue(argv[1]);
    (*argIx)++;
    m_ToFree.push_back(p);
  }
  else {
    if(opt->type == BOOL_OPT) {
      // bools are set to true when specified no more args needed.
      //opt->value = (const char *)"true";
      opt->setValue("true");
    }
    else { // Read in the next argument as being matched with option.
      checkOptType(opt, argv[1]);
      //opt->value = argv[1];
      opt->setValue(argv[1]);
      (*argIx)++;
    }
  }
}

/**
 * Match the command line arguments from
 * @param argc Number of arguments.
 * @param argv Arguments supplied to program.
 */
void PgOptions::parseOptions(int argc, const char *const argv[]) {
  int argIx = 0;

  assert(argv);
  assert(optSpec);
  progName = string(argv[0]);

  // Loop through and match options with arguments.
  for(argIx = 1; argIx < argc; argIx++) {
    const char *s = argv[argIx];
    assert(s);
    if(s[0] == '-') {
      matchOption(s, argv+argIx, &argIx);
    }
    else {
      args.push_back(argv[argIx]);
    }
  }

  // Loop through options and set those that were not specified to default.
  for(argIx = 0; optSpec[argIx] != NULL; argIx++) {
    if (optSpec[argIx]->getValue() == NULL) {
      optSpec[argIx]->resetToDefault();
    }
  }
}

/**
 * Look up the string value of an option.
 * @param name - Long name of option.
 * @return string value assocated with option.
 */
const char *PgOptions::strOpt(const char *name) {
  PgOpt *opt = mustFindPgOpt(name);
  return opt->getValue();
}

/**
 * Look up an option and give float value for it.
 * @param name - Long name of option.
 * @return float - value of option.
 */
float PgOptions::floatOpt(const char *name) {
  PgOpt *opt = mustFindPgOpt(name);
  return Convert::toFloat(opt->getValue());
}

/**
 * Look up an option and give integer value for it.
 * @param name - Long name of option.
 * @return int - value of option.
 */
int PgOptions::intOpt(const char *name) {
  PgOpt *opt = mustFindPgOpt(name);
  return Convert::toInt(opt->getValue());
}

/**
 * Look up the boolean value of an option.
 * @param name - Long name of option.
 * @return true if option set to true false otherwise.
 */
bool PgOptions::boolOpt(const char* name) {
  PgOpt *opt = mustFindPgOpt(name);
  return Convert::toBool(opt->getValue());
}

/**
 * @brief Set all the options to their default value.
 */
void PgOptions::resetToDefaults() {
  for(unsigned int i = 0; optSpec[i] != NULL; i++) {
    optSpec[i]->resetToDefault();
  }
}

void PgOptions::hack_resetOptionsList(PgOptions::PgOpt **optList)
{
  while (*optList!=NULL) {
    (*optList)->m_value=NULL;
    (*optList)->m_valueMalloced=NULL;
    (*optList)->next=NULL;
    optList++;
  }
}

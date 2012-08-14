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

#include "util/SocketHandler.h"

using namespace std;

bool SocketHandler::evenPrecedingSlash(const string &s, int pos) {
  int count = 0;
  while (--pos >= 0) {
    if (s[pos] == '\\') {
      count++;
    }
    else {
      break;
    }
  }
  return count % 2 == 0;
}

bool SocketHandler::isEscaped(const string &s, char c, int pos) {
  APT_ERR_ASSERT(s[pos] == c, "Char: " + ToStr(c) + " not found at Pos: " + ToStr(pos) + " in string: " + s);
  if (pos == 0) {
    return false;
  }
  if (pos == 1 && s[0] != '\\') {
    return false;
  }
  if (pos > 1 && ((s[pos - 1] != '\\') || evenPrecedingSlash(s, pos))) {
    return false;
  }
  return true;
}

int SocketHandler::findNextNonEscape(const string &s, char c, int start) {
  for (size_t i = start; i < s.size(); i++) {
    if (s[i] == c && !isEscaped(s, c, i)) {
      return i;
    }
  }
  return -1;
}

string SocketHandler::serializeMap(map<string,string> &pairs) {
  map<string, string>::const_iterator iter;
  string buff;
  for (iter = pairs.begin(); iter != pairs.end(); iter++) {
    APT_ERR_ASSERT(iter->first.find('=') == string::npos, "Can't have '=' delimiter in keys.");
    buff.append(iter->first);
    buff.append("=");
    buff.append(Util::escapeString(iter->second, ','));
    buff.append(",");
  }
  return buff;
}

map<string, string> SocketHandler::deserializeMap(const string &s) {
  map<string,string> pairs;
  int pos = s.find('=');
  int currentPos = 0;
  while (pos != string::npos) {
    string key = s.substr(currentPos, pos-currentPos);
    int end = findNextNonEscape(s, ',', pos+1);
    APT_ERR_ASSERT(end >=0 , "Didn't find ',' delimiter in string: " + s.substr(pos, s.size()-pos) + " for: " + s);
    string val = s.substr(pos+1, (end-pos)-1);
    val = Util::deEscapeString(val);
    pairs[key] = val;
    currentPos = end+1;
    pos = s.find('=', currentPos);
  }
  return pairs;
}

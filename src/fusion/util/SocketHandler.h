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
#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <set>
#include <string>
#include <map>

#include "util/Convert.h"
#include "util/Err.h"
#include "util/Util.h"

#define SOCKET_PROTOCOL_VER 1

/**
 * A listener class that will be called by SocketBase to parse and utilize
 * messages over a socket.
 */
class SocketHandler {

public:
  /// What type/format of message coming across
  enum MsgType {
    UNKNOWN_TYPE,
    TEXT,
    LAST_TYPE
  };
  
  /// What channel of message coming across (like stdout,stderr, etc.)
  enum MsgChannel {
    UNKNOWN_CHANNEL,
    VERBOSE,
    PROGRESS,
    KILLMSG,
    COMPLETION,
    LAST_CHANNEL
  };

  /** 
   * Determine if this handler should be called on messages from this channel
   * @param channel - What channel (e.g. SocketHandler::VERBOSE)
   * @return true if this handler is interested in messages from this channel
   */
  virtual bool handlesChannel(int channel) {
    return m_Channels.find(channel) != m_Channels.end();
  }

  /** 
   * Determine if this handler should be called on messages from this type
   * @param type - What type (e.g. SocketHandler::TEXT)
   * @return true if this handler is interested in messages from this type
   */
  virtual bool handlesType(int type) {
    return m_Types.find(type) != m_Types.end();
  }

  /** 
   * Take a message and do whatever functionality this handler
   * implements from simply displaying message to 
   * 
   * @param type - What type (i.e. text, xml, ) of message is this message.
   * @param channel - What channel (i.e. verbose, progress, etc.) is this message.
   * @param msg - Data to be parsed out
   */
  virtual void handleMessage(int type, int channel, const std::string &msg) = 0;

  /**
   * Turn a map into a serialized string like "key1=val1,key2=val2," where 
   * value strings are escaped to protect the ',' characters.
   */
  static std::string serializeMap(std::map<std::string,std::string> &pairs);

  /** 
   * Turn a serialized map back into a full map by parsing out the key=val pairs
   */
  static std::map<std::string, std::string> deserializeMap(const std::string &s);

protected:

  /**
   * Is there an even number of escape slashes preceeding position?
   */
  static bool evenPrecedingSlash(const std::string &s, int pos);

  /**
   * Is this character at position pos escaped?
   */
  static bool isEscaped(const std::string &s, char c, int pos);

  /**
   * Find the next occurance of c after start that is not escaped.
   */
  static int findNextNonEscape(const std::string &s, char c, int start);

  std::set<int> m_Channels; /// Collection of channels that a handler is interested in 
  std::set<int> m_Types; /// Collection of types that a handler is interested in

};

#endif /* SOCKETHANDLER_H */

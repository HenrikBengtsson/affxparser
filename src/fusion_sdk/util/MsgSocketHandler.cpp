////////////////////////////////////////////////////////////////
//
// Copyright (C) 2010 Affymetrix, Inc.
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

// include these before someone else does
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "util/MsgSocketHandler.h"
#include "util/SocketClient.h"
#include "util/Except.h"
#include "util/Err.h"
#include "util/Verbose.h"

#include <map>

using namespace std;

MsgSocketHandler::MsgSocketHandler(int verbosity) {
    m_LocVerbosity = MsgHandler::m_Verbosity = ProgressHandler::m_Verbosity = verbosity;
	m_Client = new SocketClient();
  }

MsgSocketHandler::~MsgSocketHandler() {
	delete m_Client;
}

void MsgSocketHandler::checkForKill() {	
    m_Client->checkForMsgs(m_Messages);
    for (size_t i = 0; i < m_Messages.size(); i++) {
		if ((m_Messages[i].find(SOCKETSTOP) != std::string::npos) ||
			(m_Messages[i].find(FATALERROR) != std::string::npos))
		{		
			Err::errAbort("ApplicationTerminated");
      }
    }
  }
 
void MsgSocketHandler::openSocket(const std::string &host, const std::string &port) {
    m_Client->socketConnect(host, port);
  }

void MsgSocketHandler::progressBegin(int verbosity, const std::string &msg, int total) {
    m_Total = total;
    if(verbosity <= m_LocVerbosity) {
      checkForKill();
      map<string,string> param;
      param["VERBOSITY"] = ToStr(verbosity);
      param["VALUE"] = "BEGIN";
      param["TOTAL"] = ToStr(total);
      param["MSG"] = msg + m_EndLine;
      string message = SocketHandler::serializeMap(param);
      m_Client->sendMsg(message, SocketHandler::TEXT, SocketHandler::PROGRESS);
    }
  }

void MsgSocketHandler::progressStep(int verbosity) {
    if(verbosity <= m_LocVerbosity) {
      checkForKill();
      map<string,string> param;
      param["VERBOSITY"] = ToStr(verbosity);
      param["VALUE"] = "STEP";
      string msg = SocketHandler::serializeMap(param);
      m_Client->sendMsg(msg, SocketHandler::TEXT, SocketHandler::PROGRESS);
    }
  }

void MsgSocketHandler::progressEnd(int verbosity, const std::string &msg) {
    if(verbosity <= m_LocVerbosity) {
      checkForKill();
      map<string,string> param;
      param["VERBOSITY"] = ToStr(verbosity);
      param["VALUE"] = "END";
      param["MSG"] = msg + m_EndLine;
      string message = SocketHandler::serializeMap(param);
      m_Client->sendMsg(message, SocketHandler::TEXT, SocketHandler::PROGRESS);
    }
  }

/** 
 * A message to be processed by the handler.
 * 
 * @param level - What level of verbosity is associated with this message, higher number == more verbosity.
 * @param msg - Message that is to be processed.
 * @param delimiter - Should a delimiter be emitted as well?
 */
void MsgSocketHandler::message(int level, const std::string &msg, bool delimiter) {
    if (level <= m_LocVerbosity) {
	  checkForKill();
      map<string,string> param;
      param["VERBOSITY"] = ToStr(level);
      param["MSG"] = msg + m_EndLine;
      string message = SocketHandler::serializeMap(param);
	  if ((message.find(SOCKETSTOP) != std::string::npos) ||
		  (message.find(FATALERROR) != std::string::npos)) {
		  	m_Client->sendMsg(message, SocketHandler::TEXT, SocketHandler::KILLMSG);
	  }
	  m_Client->sendMsg(message, SocketHandler::TEXT, SocketHandler::VERBOSE);
	}
}

void MsgSocketHandler::finishedMsg() {
  map<string,string> param;
  param["VERBOSITY"] = "0";
  param["VALUE"] = "finished";
  string message = SocketHandler::serializeMap(param);
  m_Client->sendMsg(message, SocketHandler::TEXT, SocketHandler::COMPLETION);
}

/** 
 * What level of verbosity is requested. This is a hint to the class and can
 * be ignored if desired.
 * 
 * @param level - Level below which progress messages are printed.
 */
void MsgSocketHandler::setBaseVerbosity(int level) { 
  m_LocVerbosity = level; 
}
  

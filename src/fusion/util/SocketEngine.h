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
#pragma once
#ifndef SOCKETENGINE_H
#include <string>
#include <vector>

#include "util/SocketServer.h"


class SocketTextHandler;


/**
   @brief SocketEngine design notes:
   
   The SocketEngine utilizes a simple message passing protocol over a socket to run
   an APT engine cleass in a separate process. The idea being that it is cleaner and
   easier to have GUI programs like GTC run APT programs in a separate engine rather 
   than try to run them via COM objects or managed from C# code.

   The GUI programs traditionally tie into the global Verbose object's reporting
   system. For time reasons we didn't want to rewrite all of that code so the SocketEngine
   will handle running the other process remotely and translate the messages that 
   come over the socket to the local Verbose object. This way all of the existing
   code that ties into the local Verbose object will work unchanged.

   There are two phases to the socket engine. The first is the connection of 
   the foriegn process to the local SocketServer ojbect.
   
   @image html SocketCommunicationConnect.png "Phase 1: Delegate process connects to the SocketServer in supervisor process."

   The second phase is when the delegate process passes messages back from the
   delegate process Verbose calls to the supervising process and they are
   translated to calls in the local Verbose object.

   @image html SocketCommunicationMsg.png "Phase 2: Delegate process passes messages to the supervisor process which fires the usual calls in Verbose."

 */
class SocketEngine {

public:

    SocketEngine(const std::string &program, const std::string &jobFile, 
                 const std::string &host="127.0.0.1", unsigned int port=7889);

    SocketEngine(const std::string &program, const std::vector<std::string> &args, 
                 const std::string &host="127.0.0.1", unsigned int port=7889);

    ~SocketEngine();

	SocketServer server;
	

#ifdef WIN32
	PROCESS_INFORMATION* getProcessInformation() {return &m_ProcInfo;}
	STARTUPINFO* getStartupInfo() {return &m_StartInfo;}

    void setApplicationName(LPTSTR lpApplicationName) {
        m_lpApplicationName = lpApplicationName;
    }
    void setProcessAttributes(LPSECURITY_ATTRIBUTES lpProcessAttributes) {
        m_lpProcessAttributes = lpProcessAttributes;
    }
    void setThreadAttributes(LPSECURITY_ATTRIBUTES lpThreadAttributes) {
        m_lpThreadAttributes = lpThreadAttributes;
    }
    void setInheritHandles(BOOL bInheritHandles) {
        m_bInheritHandles = bInheritHandles;
    }
    void setCreationFlags(DWORD dwCreationFlags) {
        m_dwCreationFlags = dwCreationFlags;
    }
    void setEnvironment(LPVOID lpEnvironment) {
        m_lpEnvironment = lpEnvironment;
    }
    void setCurrentDir(LPTSTR lpCurrentDirectory) {
        m_lpCurrentDirectory = lpCurrentDirectory;
    }
#endif

    bool run();
	void TerminateLaunchedApp();	
    
private:
    int doWindows(SocketServer &server, SocketTextHandler &handler, std::string &host, std::string &port,
                  std::vector<std::string> commandArgs);
    
    int doUnix(SocketServer &server, SocketTextHandler &handler, std::string &host, std::string &port,
               std::vector<std::string> commandArgs);
    
    std::string m_Program;
    std::vector<std::string> m_PgOpts;
    std::string m_Host;
    unsigned int m_Port;
    
#ifdef WIN32
    PROCESS_INFORMATION m_ProcInfo;
    STARTUPINFO m_StartInfo;
    LPTSTR m_lpApplicationName;

    LPSECURITY_ATTRIBUTES m_lpProcessAttributes;
    LPSECURITY_ATTRIBUTES m_lpThreadAttributes;
    BOOL m_bInheritHandles;
    DWORD m_dwCreationFlags;
    LPVOID m_lpEnvironment;
    LPTSTR m_lpCurrentDirectory;
	TCHAR *ws;
    
    void initVars();      
#else
    void initVars() {}; // no-op on windows
#endif /* WIN32 */
};

#endif /* SOCKETENGINE_H */

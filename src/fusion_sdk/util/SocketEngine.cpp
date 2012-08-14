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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#endif  /* WIN32 */

//
#include "util/SocketEngine.h"
//
#include "portability/affy-system-api.h"
#include "util/MsgSocketHandler.h"
#include "util/SocketTextHandler.h"
#include "util/Verbose.h"

using namespace std;

SocketEngine::SocketEngine(const std::string &program, const std::string &jobFile, 
                           const std::string &host, unsigned int port) :
    m_Program(program), m_Host(host), m_Port(port) {
    m_PgOpts.push_back(m_Program);
    m_PgOpts.push_back("--xml-file");
    m_PgOpts.push_back(jobFile);    
    initVars();
}

SocketEngine::SocketEngine(const std::string &program, const std::vector<std::string> &args, 
                           const std::string &host, unsigned int port) :
    m_Program(program),  m_Host(host), m_Port(port) {
    m_PgOpts.resize(args.size() + 1);
    m_PgOpts[0] = m_Program;
    copy(args.begin(), args.end(), m_PgOpts.begin() + 1);
    initVars();
}

SocketEngine::~SocketEngine() {
}

bool SocketEngine::run() {    
    // Our handler that will fire the appropriate verbose and progress calls locally
    SocketTextHandler handler;
    server.addHandler(&handler);
	
	unsigned int portUsed = server.socketOpen(m_Host, m_Port);
	if (portUsed == 0) {
		Verbose::out(1,"Unable to establish a socket connection.  This run will abort");
		return false;
	}
	m_Port = portUsed;	
    string port = ToStr(m_Port);
    char host_port_buf[100];
    snprintf(host_port_buf,sizeof(host_port_buf), "%s:%s", m_Host.c_str(),port.c_str());

    m_PgOpts.push_back("--console-off");
    m_PgOpts.push_back("true");
    m_PgOpts.push_back("--use-socket");
    m_PgOpts.push_back(host_port_buf);
#ifdef _WIN32
  doWindows(server, handler, m_Host, port, m_PgOpts);
#else
  doUnix(server, handler, m_Host, port, m_PgOpts);
#endif
 return handler.successfulRun();
}


int SocketEngine::doWindows(SocketServer &server, SocketTextHandler &handler, string &host, string &port,
              vector<string> commandArgs) {
#ifdef _WIN32 
 
  string command;

  for (size_t i = 0; i < commandArgs.size(); i++) {
    command = command + "\"" + commandArgs[i] + "\" ";
  }
  Verbose::out(1,"Command is: " + command);
  ws = new TCHAR[command.size()+1];
  try {
	int i=0;
	  for( std::string::const_iterator it = command.begin(); it != command.end(); it++ ) {
		//  Verbose::out(1, "Doing character: " + ToStr(i) + " of " + ToStr(command.length()));
		 ws[i++] = use_facet< ctype<wchar_t> >(cout.getloc()).widen( *it);
      }
	ws[command.size() - 1 ] = NULL;
	Verbose::out(1, "Done doing ws");

   if(!CreateProcess( m_lpApplicationName, 
                       ws,
                       m_lpProcessAttributes, 
                       m_lpThreadAttributes,
                       m_bInheritHandles,
                       m_dwCreationFlags,
                       m_lpEnvironment,
                       m_lpCurrentDirectory,
                       &m_StartInfo,
                       &m_ProcInfo)) {
      Err::errAbort("Could not run the program.");
    }
	Verbose::out(1,"Waiting for connections.");
    while(1) {
	  std::vector<std::string> messages;
      bool connected = false;
      bool beenConnected = false;
	 
      int numFound = server.acceptNewConnection();
      if (numFound > 0) {
	       Verbose::out(1, "Found connections.");     
	  }
	  DWORD exitVal;
	  bool rv = GetExitCodeProcess(m_ProcInfo.hProcess, &exitVal);
	  if (exitVal != STILL_ACTIVE) {
		  if (exitVal == 0) {
			  break;
		  }
		  else {
			  Err::errAbort("Process failed with code: " + ToStr(rv));
		  }
	  }
      if (server.isConnected()) {
        beenConnected = true;
        connected = true;
		bool stillConnected = server.processNewMessages();
		
        if (!stillConnected) {
			Verbose::out(1,"Lost connection...");
          break;
        }
      }
      if (!server.isConnected()) {
		  Verbose::out(1,".", false);
      }
      Sleep(500);
    }
  }
  catch (...) {
   TerminateProcess(m_ProcInfo.hProcess,0); 
   delete []ws;
   server.cleanUp(); 
   throw;
  }
  delete []ws;
#endif
  return 0;
}


void SocketEngine::TerminateLaunchedApp()
{
#ifdef _WIN32
	try
	{
		DWORD exitVal;
		int counter = 0;
		delete []ws;
		bool rv = GetExitCodeProcess(m_ProcInfo.hProcess, &exitVal);	
		if (exitVal == STILL_ACTIVE)
			TerminateProcess(m_ProcInfo.hProcess,0);   
	}
	catch(...)
	{
	}
#endif
 server.cleanUp();
}

int SocketEngine::doUnix(SocketServer &server, SocketTextHandler &handler, string &host, string &port,
           vector<string> commandArgs) {
  #ifndef _WIN32
  int pid = fork();
  cout << "host " << host << " port " << port;
  if(pid > 0) {
    try {
      //server.socketOpen(host, port);
      // sleep(2);
      std::vector<std::string> messages;
      bool connected = false;
      bool beenConnected = false;
      while(1) {
        int numFound = server.acceptNewConnection();
        if (numFound > 0) {
          cout << "Found " << numFound << " new connections." << endl;
        }
        if (server.isConnected()) {
          beenConnected = true;
          connected = true;
          bool stillConnected = server.processNewMessages();
          if (!stillConnected) {
            cout << "Lost connection..." << endl;
            break;
          }
        }
        if (!server.isConnected()) {
          cout << ".";
          cout.flush();
        }
        struct timespec tWait, tRet;
        tWait.tv_sec = 0;
        tWait.tv_nsec = 100000000;
        //        nanosleep(1);
        nanosleep(&tWait, &tRet);
      }
    }
    catch (...) {
      cout << "Caught exception. Killing child process: " << pid << endl;
      int rv = kill(pid, 15);
        // if termination signal doesn't work then hard kill...
      if (rv != 0) {
        rv = kill(pid, 9);
      }
    }
  }
  else if (pid == 0) {
    vector<const char*> execvp_args;
    // @todo this should be some sort of util function.
    for (int i=0;i<commandArgs.size();i++) {
      execvp_args.push_back(commandArgs[i].c_str());
    }
    execvp_args.push_back(NULL);
    char * const* tmp=(char * const*)&execvp_args[0];
    //
    cout << "Launching process..." << endl;
    int rv = execvp(tmp[0], tmp);
    cout << "command: " << commandArgs[0];
    for (int i = 1; i < commandArgs.size(); i++) {
        cout << " " << commandArgs[i];
    }
    cout << endl;
    cout << "Error - Return val: " << rv << endl;
  }
  else {
    cout << "Bad pid." << endl;
    return 1;
  }
#endif
  return 0;
}

#ifdef _WIN32
void SocketEngine::initVars() {
	Verbose::out(1, "Initializing variables.");
    memset(&m_StartInfo,0,sizeof(m_StartInfo));
    m_StartInfo.cb= sizeof(m_StartInfo);
    memset(&m_ProcInfo,0,sizeof(m_ProcInfo));
	m_lpApplicationName = NULL;
    m_lpProcessAttributes = NULL;
    m_lpThreadAttributes = NULL;
    m_bInheritHandles = false;
    m_dwCreationFlags = CREATE_NO_WINDOW;
    m_lpEnvironment = NULL;
    m_lpCurrentDirectory = NULL;

}
#endif  /*WIN32 */

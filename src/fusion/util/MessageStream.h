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

/* \file MessageStream.h Provides a base class for streaming messages from a component to its calling function. */

#ifndef _MessageStream_HEADER_
#define _MessageStream_HEADER_

#include <cstring>
#include <string>
//

/*! Provides a base class definition for streaming back messages to the parent process. */
class MessageStream
{
protected:
	/*! The level of messages to display. Larger number equals more messages. */
	int verbosity;

public:

	/*! Constructor */
	MessageStream() { verbosity = 0; }

  virtual ~MessageStream() { }

	/*! Set the level of verbosity desired. 0 == no messages
	 * 1 == normal messages, 2,3,4, etc. == more verbose.
	 * @param level - level of verbosity desired.
	 */  
	void SetLevel(int level) { verbosity = level; }

	/*! Get the level of verbosity desired. 0 == no messages
	 * 1 == normal messages, 2,3,4, etc. == more verbose.
	 * @return current level of verbosity
	 */  
	int GetLevel() const { return verbosity;}

	/*! Send a message.
	 * @param level - level of verbosity desired.
	 * @param msg The message to send.
	 */
	virtual void Write(int level, const std::string &msg) = 0;
};

#endif

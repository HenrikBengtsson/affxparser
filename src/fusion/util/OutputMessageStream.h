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

/* \file OutputMessageStream.h Provides streaming messages to a ofstream from a component to its calling function. */

#ifndef _OutputMessageStream_HEADER_
#define _OutputMessageStream_HEADER_

#include "util/MessageStream.h"
//
#include <iostream>
#include <ostream>
//

/*! Provides a base class definition for streaming back messages to the parent process. */
class OutputMessageStream : public MessageStream
{
private:
	/*! Where to stream the messages. */
	std::ostream *outstr;

public:
	/*! Construct a class with the type of output. 
	 * @param level - level of verbosity desired.
	 * @param strm The output stream.
	 */
	OutputMessageStream(int level, std::ostream *strm = &std::cout);

	/*! Send a message.
	 * @param level - level of verbosity desired.
	 * @param msg The message to send.
	 */
	void Write(int level, const std::string &msg);
};

#endif

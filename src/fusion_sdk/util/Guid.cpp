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

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#ifdef WIN32
#pragma warning(disable: 4996) // ignore deprecated functions warning
#define snprintf _snprintf
//need to include winsock2 before windows.h (pulled in by affy-base-types)
#include <winsock2.h>
#include <process.h>
#define getpid _getpid
#define gethostid() 0
#else
#include <unistd.h>
#endif

#include "Guid.h"
#include "chksum.h"


#define SEEDVAL(VAL) do { \
  unsigned int val=(unsigned int)(VAL); \
  if (val!=0) { \
    seed=seed*val; \
  } \
} while (0)

using namespace affxutil;

/*
 * Initialize the socket library.
 */
/// @todo need more randomness here.
Guid::Guid()
{
#ifdef WIN32
	// initialize Winsock library
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	int nResult = WSAStartup(wVersionRequested, &wsaData);
#endif


	// Since we are using "*" we want to avoid zero...

	// the pid is included for the case where two processes
	// start up in the same second. 
	unsigned int seed;
	// start with a number...
	seed=314159; 
	// add more sources of randomness here.
	SEEDVAL(time(NULL));
	SEEDVAL(getpid());
	SEEDVAL(gethostid()); // this is zero on darwin
	srand(seed);

}

/*
 * Clean up the socket library.
 */
Guid::~Guid()
{
#ifdef WIN32
	WSACleanup();
#endif
}

/*
 * Create a new guid based on the host name, current time and random numbers.
 * A checksum of the string values are taken so as to remove any user
 * interpretive information. This should strictly just be an identifier.
 */
GuidType Guid::GenerateNewGuid()
{
	const int GUID_LENGTH = 54;
	char guid[GUID_LENGTH+1];
	time_t currentTime;
	const int MAX_HOST_NAME = 64;
	char hostname[MAX_HOST_NAME];
        static bool seeded = false;
	currentTime = time(NULL);
        /* Make sure we're not seeding with 1 every time (default on linux) */
        if(!seeded) {
          srand(currentTime);
          seeded = true;
        }
	gethostname(hostname, MAX_HOST_NAME);

  /// @todo This is bad -- consectuive calles to "rand()" are not random.
  /// The values should be hashed with something stronger than ones complement.
	snprintf(guid,sizeof(guid), "%10d-%10d-%10d-%10d-%10d",
		CheckSum::OnesComplementCheckSum(hostname, strlen(hostname)/2),
		(int) currentTime, rand(), rand(), rand());
	guid[GUID_LENGTH] = 0;
	for (int i=0; i<GUID_LENGTH; i++)
	{
		if (guid[i] == ' ')
			guid[i] = '0';
	}

	return GuidType(guid);
}

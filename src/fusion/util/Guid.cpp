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

#ifdef _WIN32
#include <winsock2.h> 
#endif
//
#include "util/Guid.h"
//
#include "portability/affy-system-api.h"
#include "util/Convert.h"
#include "util/Err.h"
#include "util/chksum.h"
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
//

//////////

using namespace affxutil;

//////////

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Since we are using "*" we want to avoid zero when mixing in data to the seed...
#define AFFY_SEEDVAL(VAL) do { \
  unsigned int val=(unsigned int)(VAL); \
  if (val!=0) { \
    seed=seed*val; \
  } \
} while (0)

/// Have we seeded srand already?
static int affy_seed_srand_done=0;

void affxutil::affy_seed_srand() {
	// the pid is included for the case where two processes
	// start up in the same second. 
	unsigned int seed;
	// start with a number...
	seed=314159; 
	// add more sources of randomness here.
	AFFY_SEEDVAL(time(NULL));
	AFFY_SEEDVAL(getpid());
  // gethostid is zero on darwin, but that is ok.
	AFFY_SEEDVAL(gethostid());

  // Check the env var for a seed (for debugging)
#ifndef WIN32
  char* debug_seed_str=getenv(AFFY_DEBUG_SRAND_SEED_ENVVAR);
  if (debug_seed_str!=NULL) {
    char* endptr;
    unsigned int debug_seed=strtol(debug_seed_str,&endptr,0);
    // did we parse the entire string?
    if ((endptr==debug_seed_str)||(*endptr!=0)) {
      // nope!
      Err::errAbort("Unable to parse env var: '"+ToStr(AFFY_DEBUG_SRAND_SEED_ENVVAR)+"'='"+ToStr(debug_seed_str)+"'");
    }
    /// @todo: log the seed and tell the user we are using a debugging seed.
    // set it...
    seed=debug_seed;
  }
#endif

  // tell srand our seed value...
	srand(seed);
  // remember that the seeding has been done...
  affy_seed_srand_done=1;
}

/// @brief     Ensure that the seed as been set once.
void affxutil::ensure_srand_seeded() {
  if (affy_seed_srand_done==0) {
    affxutil::affy_seed_srand();             
  }
}

Guid::Guid()
{
#ifdef WIN32
	// Initialize Winsock library
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	int nResult = WSAStartup(wVersionRequested, &wsaData);
#endif

  affxutil::ensure_srand_seeded();
}

Guid::~Guid()
{
  // Clean up the socket library.
#ifdef WIN32
	WSACleanup();
#endif
}


// Check which format to use...
#ifdef AFFY_GUID_FORMAT_RFC4122

// See the following references:
//   http://www.ietf.org/rfc/rfc4122.txt
//   http://en.wikipedia.org/wiki/Globally_Unique_Identifier

GuidType Guid::GenerateNewGuid()
{
  affxutil::ensure_srand_seeded();

  // generate the random bits of the guid...
  int data1=rand();
  int data2=(rand()%0xFFFF);
  int data3=rand();
  // mark this guid as version 4 (a pseudo-random guid)
  // the version is in the high byte.
  data3=data3&0x0FFF;
  data3=data3|0x4000; 
  // data4 is 8 bytes. We generate it in three groups.
  int data41=(rand()&0xFFFF);   // 2B
  int data42=(rand()&0xFFFFFF); // 3B
  int data43=(rand()&0xFFFFFF); // 3B

  // f81d4fae-7dec-11d0-a765-00a0c91e6bf6
  char buf[100];
  snprintf(buf,sizeof(buf),"%08x-%04x-%04x-%04x-%06x%06x",
           data1,data2,data3,data41,data42,data43);
  // std::stringify it.
  GuidType guid=buf;
  return guid;
}

#else
/*
 * Create a new guid based on the host name, current time and random numbers.
 * A checksum of the string values are taken so as to remove any user
 * interpretive information. This should strictly just be an identifier.
 */
GuidType Guid::GenerateNewGuid()
{
	const int GUID_LENGTH = 54;
	char guid[GUID_LENGTH+1];
	const int MAX_HOST_NAME = 64;
	char hostname[MAX_HOST_NAME];
	time_t currentTime = time(NULL);

  affxutil::ensure_srand_seeded();

	gethostname(hostname, MAX_HOST_NAME);

  /// @todo This is bad -- consecutive calles to "rand()" are not random.
  ///       The values should be hashed with something stronger than ones complement.
  /// @todo printf will pad with "0" if the format string has a leading "0".
  ///       The format string should be: "%010d-%010d-%010d-%010d-%010d",
  ///       And is this what a uuid is supposed to look like?
  ///       'uuidgen' prints a value like: "a5b4b2ca-006d-4b89-b7ca-14eedb1e02b9"
	snprintf(guid,sizeof(guid), "%10d-%10d-%10d-%10d-%10d",
		CheckSum::OnesComplementCheckSum(hostname, strlen(hostname)/2),
		(int) currentTime, rand(), rand(), rand());
	guid[GUID_LENGTH] = 0;

  // fill with zeros
	for (int i=0; i<GUID_LENGTH; i++)	{
		if (guid[i] == ' ')
			guid[i] = '0';
	}

	return GuidType(guid);
}

#endif

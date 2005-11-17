/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "AffymetrixGuid.h"
#include "checksum.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#ifdef WIN32
#pragma warning(disable: 4996) // ignore deprecated functions warning
#include <winsock2.h>
#include <process.h>
#define getpid _getpid
#endif

#ifndef WIN32
#include <unistd.h>
#endif

using namespace affymetrix_calvin_utilities;

/*
 * Initialize the seed
 */
void InitializeRandomNumberGenerator()
{
	// Get the current time as a starting point for a seed.
	unsigned int seed = (unsigned int) time(NULL);

	// Multiply the time by its address and the process id.
	seed *= (unsigned int)(&seed);
	seed *= getpid();

	// Initialize the random number generator.
	srand(seed);
}

/*
 * Initialize the socket library.
 */
AffymetrixGuid::AffymetrixGuid()
{
#ifdef WIN32
	// initialize Winsock library
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	int nResult = WSAStartup(wVersionRequested, &wsaData);
#endif
	InitializeRandomNumberGenerator();
}

/*
 * Clean up the socket library.
 */
AffymetrixGuid::~AffymetrixGuid()
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
AffymetrixGuidType AffymetrixGuid::GenerateNewGuid()
{
	const int GUID_LENGTH = 54;
	char guid[GUID_LENGTH+1];
	time_t currentTime;
	const int MAX_HOST_NAME = 64;
	char hostname[MAX_HOST_NAME];

	currentTime = time(NULL);
	gethostname(hostname, MAX_HOST_NAME);

	sprintf(guid, "%10d-%10d-%10d-%10d-%10d",
		CheckSum::OnesComplementCheckSum(hostname, strlen(hostname)/2),
		(int) currentTime, rand(), rand(), rand());
	guid[GUID_LENGTH] = 0;
	for (int i=0; i<GUID_LENGTH; i++)
	{
		if (guid[i] == ' ')
			guid[i] = '0';
	}

	return AffymetrixGuidType(guid);
}

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
#include "windows.h"
#endif

#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include "calvin_files/utils/src/checksum.h"
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string.h>
#include <string>
#include <stdio.h>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // ignore deprecated functions warning
#include <process.h>
#define getpid _getpid
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif

using namespace affymetrix_calvin_utilities;

/*
 * Initialize the seed
 */
void InitializeRandomNumberGenerator()
{
	// Get the current time as a starting point for a seed.
	u_int64_t seed = (u_int64_t) time(NULL);

	// Multiply the time by its address and the process id.
	seed *= (u_int64_t)(&seed);
	seed *= (u_int64_t)getpid();

	// Initialize the random number generator.
	srand((unsigned int)seed);
}

/*
 * Initialize the socket library.
 */
AffymetrixGuid::AffymetrixGuid()
{
#ifdef _MSC_VER
	// initialize Winsock library
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSAStartup(wVersionRequested, &wsaData);
#endif
	InitializeRandomNumberGenerator();
}

/*
 * Clean up the socket library.
 */
AffymetrixGuid::~AffymetrixGuid()
{
#ifdef _MSC_VER
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

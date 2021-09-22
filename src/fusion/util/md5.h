/* This code was imported from OpenBSD.  It is on the
 * COLIN_PLUMB_MD5 branch should it need updating. -jhg
 *
 * Before using these functions directly look at
 * "md5sum.cpp" which defines an "md5sum" class which should
 * make things eaiser to use from c++.
 */

/*	$OpenBSD: md5.h,v 1.1.2.1 2004/06/05 23:12:36 niklas Exp $	*/

/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 */

#ifndef _MD5_H_
#define _MD5_H_

//
#include <portability/affy-base-types.h>
//
#include <cstdlib>
//

// some typedefs to smooth things over.
typedef uint8_t   u_int8_t;
typedef uint32_t  u_int32_t;
typedef uint64_t  u_int64_t;

#define	MD5_BLOCK_LENGTH		64
#define	MD5_DIGEST_LENGTH		16

/// @brief The MD5 state context
typedef struct MD5Context {
	u_int32_t state[4];			///< state
	u_int64_t count;        ///< number of bits, mod 2^64
	u_int8_t buffer[MD5_BLOCK_LENGTH];	///< input buffer
} MD5_CTX;

void	 MD5Init(MD5_CTX *);
void	 MD5Update(MD5_CTX *, const u_int8_t *, u_int32_t);
void	 MD5Final(u_int8_t [MD5_DIGEST_LENGTH], MD5_CTX *);
void	 MD5Transform(u_int32_t [4], const u_int8_t [MD5_BLOCK_LENGTH]);

#endif /* _MD5_H_ */

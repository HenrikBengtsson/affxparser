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


#ifndef _FileUtils_HEADER_
#define _FileUtils_HEADER_

/*! \file FileUtils.h This file provides file utilities.
 */

#include <cstring>
#include <fstream>
#include <list>
#include <string>
//

namespace affymetrix_calvin_utilities
{

/*! This class provides utility functions for files. */
class FileUtils
{
public:
	/*! Checks for the existance of a file.
	 *
	 * @param fileName The name of the file to test for existance.
	 * @return True if the file exists.
	 */
	static bool Exists(const char *fileName);

	/*! Locks a file for exclusive use.
	 * This is a soft locking mechanism. The actual file is not locked, instead
	 * it uses a "LCK" file to determine the lock status.
	 *
	 * @param fileName The name of the file to lock.
	 * @return True if the file was successfully locked.
	 */
	static bool LockFile(const char *fileName);

	/*! Unlocks a file for exclusive use.
	 * This is a soft locking mechanism. The actual file is not locked, instead
	 * it uses a "LCK" file to determine the lock status.
	 *
	 * @param fileName The name of the file to unlock.
	 * @return True if the file was successfully unlocked.
	 */
	static bool UnlockFile(const char *fileName);

	/*! Retrieves the list of files in a path that match the extension.
	 *
	 * @param pathName The path to search.
	 * @param ext The file extension.
	 * @return The list of files (full path) found in the directory.
	 */
	static std::list<std::string> ListFiles(const char *pathName, const char *ext);

	/*! Deletes a file.
	 * @param fileName The name of the file to delete.
	 * @return True if the file was deleted.
	 */
	static bool RemoveFile(const char *fileName);

	/*! Deletes a file.
	 * @param path The name of the path to delete.
	 * @return True if the path was deleted.
	 */
	static void RemovePath(const char *path);
};

};

#endif // _FileUtils_HEADER_

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

/**
 * @file   SQLite.h
 * @author Walt Short
 * @date   Fri Sept 28 15:24:08 2007
 * 
 * @brief  Wrapper for SQLite.
 * 
 */

#ifndef _SQLITE_H_
#define _SQLITE_H_

#include "../../../external/sqlite/sqlite3.h"
//
#include <sstream>
//

class SQLiteDatabase;

/**
 *  SQLiteException - Exception class for SQLite.
 */
class SQLiteException
{
public:
	std::string m_strMessage;

	/** 
	 * Constructs a SQLiteException object.
	 * @param strMessage - The error message.
	 */
	SQLiteException(const std::string& str) {m_strMessage = str;}
	
	/** 
	 * Returns the error message.
	 * @return - The error message string.
	 */
	std::string getMessage() {return m_strMessage;}
};

/**
 *  SQLiteRecordset - Class for handling query result sets.
 */
class SQLiteRecordset
{
protected:
	SQLiteDatabase* m_pdb;
	sqlite3_stmt* m_pstmt;
	bool m_bFirstRow;
	bool m_bEOF;
	bool m_bOpen;

public:
	SQLiteRecordset(SQLiteDatabase& db);
	virtual ~SQLiteRecordset();

	void open(const std::string& strSQL);
	void close();
	bool fetch();

	std::string getString(int iColumnIndex);
	int getInteger(int iColumnIndex);
	double getDouble(int iColumnIndex);
};

/**
 *  SQLiteDatabase - Class for handling SQLite databases.
 */
class SQLiteDatabase
{
public:
	SQLiteDatabase();
	virtual ~SQLiteDatabase();

protected:
        std::string m_dbName;
	sqlite3* m_pdb;
	sqlite3_stmt* m_pstmt;
	bool m_bOpen;
	
public:	
	static void test();
	/** 
	 * Returns the sqlite3 reference.
	 * @return - The sqlite3 reference.
	 */
	sqlite3& getConnection() {return *m_pdb;}
	void error(int iReturnCode, const std::string& strMessage);
	void open(const std::string& strFileName, bool bReadOnly = false);
	void close();
	void execute(const std::string& strSQL, bool bError = true, bool abortOnErr = true);
	SQLiteRecordset* newRecordset() {return new SQLiteRecordset(*this);}
	void deleteRecordset(SQLiteRecordset** pprset) {delete *pprset; *pprset = NULL;} 
	
	/** 
	 * Begin a transaction.
	 */
	void beginTransaction() {execute("begin");}

	/** 
	 * Commit a transaction.
	 */
	void commitTransaction() {execute("commit");}

	/** 
	 * Rollback a transaction.
	 */
	void rollbackTransaction() {if (m_bOpen) {execute("rollback", false);}}
};

#endif



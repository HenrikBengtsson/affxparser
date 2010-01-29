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

/**
 * @file   SQLite.cpp
 * @author Walt Short
 * @date   Fri Sept 28 15:24:08 2007
 * 
 * @brief  Wrapper for SQLite.
 * 
 */

#include "util/AffxConv.h"
#include "util/AffxString.h"
#include "util/Verbose.h"
//
#include "util/SQLite.h"
//

// A test function.
void SQLiteDatabase::test()
{
//  Sample usage code follows...
	std::string strFileName = "sqlite3.txt";	
	SQLiteDatabase db;	
	try	{
	Verbose::out(1, "*");
	Verbose::out(1, "Create/Open database: sqlite3.txt");
	db.open(strFileName);
	SQLiteRecordset rset(db);

	Verbose::out(1, "create table Test1 (Id int primary key, Name varchar(255))");
	db.execute("create table Test1 (Id int primary key, Name varchar(255))");
	Verbose::out(2, "create table Test1 (Id int primary key, Name varchar(255))");
	db.execute("create table Test2 (Id int primary key, Name varchar(255))");
	
	Verbose::out(1, "Begin transaction...");
	db.beginTransaction();
	for (int iIndex = 1; (iIndex < 10); iIndex++)
	{
		std::string strSQL = "insert into Test1 values (" + ::getInt(iIndex) + ", 'Test1-" + ::getInt(iIndex) + "')";
		Verbose::out(1, "\t" + strSQL);
		db.execute(strSQL);
	}
	for (int iIndex = 1; (iIndex < 10); iIndex++)
	{
		std::string strSQL = "insert into Test2 values (" + ::getInt(iIndex) + ", 'Test2-" + ::getInt(iIndex) + "')";
		Verbose::out(1, "\t" + strSQL);
		db.execute(strSQL);
	}
	Verbose::out(1, "Commit transaction...");
	db.commitTransaction();
	
	Verbose::out(1, "select Test1.id, Test1.Name, Test2.Name from Test1, Test2 where Test1.Id = Test2.id");
	rset.open("select Test1.id, Test1.Name, Test2.Name from Test1, Test2 where Test1.Id = Test2.id");
	while (rset.fetch())
	{
		int iID = rset.getInteger(0);
		std::string strName = rset.getString(1);
		std::string strName2 = rset.getString(2);
		Verbose::out(1, "\tfetched: " + ::getInt(iID) + ", " + strName + ", " + strName2);
	}
	rset.close();	
	
	Verbose::out(1, "drop table Test1");
	db.execute("drop table Test1");
	Verbose::out(1, "drop table Test2");
	db.execute("drop table Test2");
	
	Verbose::out(1, "Closing SQLite database");
	db.close();
	Verbose::out(1, "*");
	} catch (SQLiteException& e) {db.rollbackTransaction(); db.close(); Verbose::out(1, e.getMessage());} 
//  End sample usage code
}

/** 
 * Constructs a SQLiteDatabase object.
 */
SQLiteDatabase::SQLiteDatabase()
{
	m_pdb = NULL;
	m_pstmt = NULL;
	m_bOpen = false;
}

/** 
 * Destroys a SQLiteDatabase object.
 * Closes the database if it is left open.
 */
SQLiteDatabase::~SQLiteDatabase()
{
	close();
}

/** 
 * Error handling function. Throws a SQLiteException pointer.
 * The error code and Message will be combined into the SQLiteException message.
 * @param iReturnCode - The error code from SQLite.
 * @param strMessage - The error message.
 */
void SQLiteDatabase::error(int iReturnCode, const std::string& strMessage) 
{ 
	std::stringstream ss;
	std::string str;
	ss << iReturnCode;
	ss >> str;
	std::string strMsg = "ERROR: SQLiteCode: " + str + ", Message: " + strMessage;
	throw SQLiteException(strMsg);

} 

/** 
 * Opens a SQLite database.
 * @param strFileName - The name of the file to open.
 * @param bReadOnly - true if the file is to be opened read only, else false. (Defaults to false.)
 */
void SQLiteDatabase::open(const std::string& strFileName, bool bReadOnly)
{
	/*
	int iFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	if (bReadOnly) {iFlags = SQLITE_OPEN_READONLY;}
	int iReturnCode = sqlite3_open_v2(strFileName.c_str(), &m_pdb, iFlags, NULL);
	if (iReturnCode != SQLITE_OK) {error(iReturnCode, "Failed to open SQLite file:\t" + strFileName);}
	*/
	int iReturnCode = sqlite3_open(strFileName.c_str(), &m_pdb);
	if (iReturnCode != SQLITE_OK) {error(iReturnCode, "Failed to open SQLite file:\t" + strFileName);}
	m_bOpen = true;
}

/** 
 * Closes a SQLite database.
 */
void SQLiteDatabase::close()
{
	if (m_bOpen)
	{
		int iReturnCode = sqlite3_close(m_pdb);
		if (iReturnCode != SQLITE_OK) {error(iReturnCode, "Failed to close SQLite file.");}
		m_bOpen = false;
	}
}

/** 
 * Executes a SQL statement. Does not return any data.
 * @param strSQL - The SQL statement to execute.
 * @param bError - Set to true to trap errors, or false to ignore errors.
 */
void SQLiteDatabase::execute(const std::string& strSQL, bool bError)
{
	if (m_pdb == NULL) {throw SQLiteException("ERROR: SQLite Database has not been opened."); }
	char* pszMsg = NULL;
	int iReturnCode = sqlite3_exec(m_pdb, strSQL.c_str(), NULL, NULL, &pszMsg);
	if (bError)
	{
		if (iReturnCode != SQLITE_OK) {std::string str = pszMsg; sqlite3_free(pszMsg); error(iReturnCode, str);}
	}
	else
	{
		if (iReturnCode != SQLITE_OK) {sqlite3_free(pszMsg);}
	}
}

/** 
 * Constructs a SQLiteRecordset object.
 * @param pdb - The pointer to the SQLiteDatabase.
 */
SQLiteRecordset::SQLiteRecordset(SQLiteDatabase& db)
{
	m_pdb = &db;
	m_bFirstRow = false;
	m_bEOF = true;
	m_bOpen = false;
}

/** 
 * Destroys a SQLiteRecordset object.
 * Closes the recordset if left open.
 */
SQLiteRecordset::~SQLiteRecordset()
{
	close();
}

/** 
 * Prepares a SQL statement.
 * @param strSQL - The SQL statement.
 */
void SQLiteRecordset::open(const std::string& strSQL)
{
	const char* pszTail = NULL;
	int iReturnCode = sqlite3_prepare_v2(&(m_pdb->getConnection()), strSQL.c_str(), (int)strSQL.length(), &m_pstmt, &pszTail);
	if (iReturnCode != SQLITE_OK) {m_pdb->error(iReturnCode, "Failed to prepare SQL statement.");}
	m_bOpen = true;
	m_bFirstRow = true;
	m_bEOF = false;
}

/** 
 * Closes the SQLiterecordset.
 */
void SQLiteRecordset::close()
{
	if (m_bOpen)
	{
		int iReturnCode = sqlite3_finalize(m_pstmt);
		if (iReturnCode != SQLITE_OK) {m_pdb->error(iReturnCode, "Failed to finalize SQL statement.");}
		m_bOpen = false;
		m_bFirstRow = false;
	}
}

/** 
 * Steps through the result set from a prepared SQL statement.
 * @return - bool true if more rows to fetch, or false if the end of the recordset has been reached.
 */
bool SQLiteRecordset::fetch() 
{
	int iReturnCode = sqlite3_step(m_pstmt);  
	if (iReturnCode != SQLITE_ROW) {m_bEOF = true;} 
	return !m_bEOF;
}

/** 
 * Get a String value from the recordset.
 * @param iColumnIndex - The zero based index of the column.
 * @return - String value retrieved from column.
 */
std::string SQLiteRecordset::getString(int iColumnIndex) 
{	
	AffxString str;
	char* p = (char*)sqlite3_column_text(m_pstmt, iColumnIndex);
	if (p != NULL) {str = p;}
	return str;
}

/** 
 * Get an integer value from the recordset.
 * @param iColumnIndex - The zero based index of the column.
 * @return - integer value retrieved from column.
 */
int SQLiteRecordset::getInteger(int iColumnIndex) 
{
	return sqlite3_column_int(m_pstmt, iColumnIndex);
} 

/** 
 * Get a Double value from the recordset.
 * @param iColumnIndex - The zero based index of the column.
 * @return - Double value retrieved from column.
 */
double SQLiteRecordset::getDouble(int iColumnIndex) 
{
	return sqlite3_column_double(m_pstmt, iColumnIndex);
} 

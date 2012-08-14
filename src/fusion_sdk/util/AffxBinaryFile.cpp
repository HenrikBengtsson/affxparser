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
 * @file AffxBinaryFile.cpp
 *
 * @brief This file contains the AffxBinaryFile class members. 
 */

#include "util/AffxBinaryFile.h"

AffxBinaryFile::AffxBinaryFile()
{
	m_pistrm = NULL;
	m_postrm = NULL;
}

AffxBinaryFile::~AffxBinaryFile()
{
	close();
}

bool AffxBinaryFile::open(const AffxString& strFileName, m_enumOpenOptions enumOpenOption)
{
	std::fstream fstrm;
	switch (enumOpenOption)
	{
	case LOAD:
		fstrm.open(strFileName.c_str(), std::fstream::in | std::fstream::binary);
		if (fstrm.is_open()) {fstrm.close();} else {return false;}
		m_pistrm = new std::ifstream(strFileName.c_str(), std::ios::in | std::ios::binary);
		if (m_pistrm == NULL) {return false;}
		break;
	case SAVE:
		m_postrm = new std::ofstream(strFileName.c_str(), std::ios::out | std::ios::binary);
		if (m_postrm == NULL) {return false;}
		break;
	case APPEND:
		m_postrm = new std::ofstream(strFileName.c_str(), std::ios::out | std::ios::binary | std::ios::app);
		if (m_postrm == NULL) {return false;}
		break;
	default:
		return false;
	}
	return true;
}

void AffxBinaryFile::close()
{
	if (m_pistrm != NULL) {m_pistrm->close(); delete m_pistrm; m_pistrm = NULL;}
	if (m_postrm != NULL) {m_postrm->close(); delete m_postrm; m_postrm = NULL;}
}

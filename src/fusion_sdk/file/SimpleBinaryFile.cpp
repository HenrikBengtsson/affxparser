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

#include "file/SimpleBinaryFile.h"
//
#include "file/FileIO.h"
#include "file/FileWriter.h"
//

SimpleBinaryFile::SimpleBinaryFile()
{
}

SimpleBinaryFile::~SimpleBinaryFile()
{
}

bool SimpleBinaryFile::CreateNewFile(const std::string &fileName)
{
    outFile.open(fileName.c_str(), std::ios::out | std::ios::binary);
    return (outFile ? true : false);
}

void SimpleBinaryFile::WriteHeader(int rows, int cols, const std::string &rowHeaderLabel, int rowHeaderLabelMaxLength, const std::vector<std::string> &colIds, const std::vector<std::string> &colNames)
{
    rowHeaderLength = rowHeaderLabelMaxLength;
    WriteInt32_N(outFile, rows);
    WriteInt32_N(outFile, cols);
    WriteString_N(outFile, rowHeaderLabel);
    WriteInt32_N(outFile, rowHeaderLength);
    for (int icol=0; icol<cols; icol++)
        WriteString_N(outFile, colIds[icol]);
    for (int icol=0; icol<cols; icol++)
        WriteString_N(outFile, colNames[icol]);
}

bool SimpleBinaryFile::Close()
{
    if (outFile.is_open())
    {
        outFile.close();
        return !outFile.fail();
    }
    if (inFile.is_open())
    {
        inFile.close();
        return !inFile.fail();
    }
    return false;
}

void SimpleBinaryFile::WriteRow(const std::string &rowLabel, const std::vector<float> &values)
{
    WriteRowLabel(rowLabel);
    for (std::vector<float>::const_iterator it=values.begin(); it!=values.end(); ++it)
        WriteFloat_N(outFile, *it);
}

void SimpleBinaryFile::WriteRowLabel(const std::string &rowLabel)
{
    WriteFixedString(outFile, rowLabel, rowHeaderLength);
}

void SimpleBinaryFile::WriteData(float data)
{
    WriteFloat_N(outFile, data);
}

void SimpleBinaryFile::UpdateRowCount(const std::string &fileName, int rows)
{
    std::ofstream fileStr; 
    fileStr.open(fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary);
    WriteInt32_N(fileStr, rows);
    fileStr.close();
}

bool SimpleBinaryFile::OpenFile(const std::string &fileName, int &rows, int &cols, std::string &rowHeaderLabel, std::vector<std::string> &colNames)
{
    inFile.open(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!outFile)
        return false;
    ReadInt32_N(inFile, rows);
    ReadInt32_N(inFile, cols);
    ReadString_N(inFile, rowHeaderLabel);
    ReadInt32_N(inFile, rowHeaderLength);
    colNames.resize(cols);
    for (int icol=0; icol<cols; icol++)
        ReadString_N(inFile, colNames[icol]);
    dataStartPos = inFile.tellg();
    rowSize = rowHeaderLength + (sizeof(float) * cols);
    return true;
}

void SimpleBinaryFile::ReadRow(int row, std::string &rowLabel, std::vector<float> &data)
{
    inFile.seekg(dataStartPos + row*rowSize);
    ReadFixedString(inFile, rowLabel, rowHeaderLength);
    for (int i=0; i<(int)data.size(); i++)
        ReadFloat_N(inFile, data[i]);
}

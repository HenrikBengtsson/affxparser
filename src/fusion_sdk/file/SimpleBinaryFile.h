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
#ifndef _SimpleBinaryFile_HEADER_
#define _SimpleBinaryFile_HEADER_

/**
 * @file SimpleBinaryFile.h Provides interfaces to create a simple binary file 
 *       storing a table of data. 
 */

//
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
//

/*! Used to create a simple binary file storing a table of data. */
class SimpleBinaryFile
{
public:
    /*! Constructor */
    SimpleBinaryFile();
    
    /*! Destructor */
    ~SimpleBinaryFile();

    /*! Create a new file.
     * @param fileName The file name.
     * @return True if created.
     */
    bool CreateNewFile(const std::string &fileName);

    /*! Write the header
     * @param rows The number of rows
     * @param cols The number of columns of data (not including the row label.
     * @param rowHeaderLabel The label for the row header.
     * @param rowHeaderLabelMaxLength The maximum length of the row header values
     * @param colIds A unique id for each column
     * @param colNames The names of each column
     */
    void WriteHeader(int rows, int cols, const std::string &rowHeaderLabel, int rowHeaderLabelMaxLength, const std::vector<std::string> &colIds, const std::vector<std::string> &colNames);

    /*! Write a row of data.
     * @param rowLabel The label for the row
     * @param values The column data
     */
    void WriteRow(const std::string &rowLabel, const std::vector<float> &values);

    /*! Write a row label.
     * @param rowLabel The label for the row
     */
    void WriteRowLabel(const std::string &rowLabel);

    /*! Write a data point.
     * @param data The data to write.
     */
    void WriteData(float data);

    /*! Close the file and check the status. */
    bool Close();

    /*! Update the row count.
     * @param fileName The name of the file.
     * @param rows The row count.
     */
    void UpdateRowCount(const std::string &fileName, int rows);

    /*! Open an existing file.
     * @param fileName The file name.
     * @param rows The number of rows
     * @param cols The number of columns of data (not including the row label.
     * @param rowHeaderLabel The label for the row header.
     * @param colNames The names of each column
     * @return True if successful.
     */
    bool OpenFile(const std::string &fileName, int &rows, int &cols, std::string &rowHeaderLabel, std::vector<std::string> &colNames);

    /*! Read a row of data
     * @param row The row index
     * @param rowLabel The row label
     * @param data The data
     */
    void ReadRow(int row, std::string &rowLabel, std::vector<float> &data);

private:
    /*! The output stream. */
    std::ofstream outFile;

    /*! The input stream. */
    std::ifstream inFile;

    /*! The length of the row headers. */
    int rowHeaderLength;

    /*! The position where the data starts. */
    int dataStartPos;

    /*! The size of each row. */
    int rowSize;
};

#endif

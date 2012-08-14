////////////////////////////////////////////////////////////////
//
// Copyright (C) 1989, 1991 Free Software Foundation, Inc.
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

//
#include "util/CalvinToText.h"
//
#include "broadutil/BroadException.h"
#include "calvin_files/exception/src/ExceptionBase.h"
#include "util/Fs.h"
#include "util/Util.h"

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;


// Constructor.
CalvinToTextFile::CalvinToTextFile()
{
  m_cb = NULL;	
  m_pfileStream = NULL;
}

// Destructor.
CalvinToTextFile::~CalvinToTextFile()
{
  if (isOpen()) {
    m_pfileStream->close();
    delete m_pfileStream;
  }
  if (m_cb != NULL) {delete[] m_cb;}
}

bool CalvinToTextFile::isOpen() {
  return ((m_pfileStream != NULL) && (m_pfileStream->is_open()));
}

void CalvinToTextFile::close(void)
{
  if (isOpen()) {
      m_pfileStream->close();
      delete m_pfileStream;
      m_pfileStream =  NULL;
      if (m_cb != NULL) {delete[] m_cb; m_cb = NULL;}
  }
}


// Open the file.
bool CalvinToTextFile::open(const AffxString& strFileName ) 
{

  m_pfileStream = new std::fstream;

  Fs::aptOpen(*m_pfileStream, strFileName, fstream::out | fstream::binary | fstream::trunc);

  if (!isOpen()) {
    delete m_pfileStream;
    m_pfileStream = NULL;
  }
  return isOpen();
}



void CalvinToTextFile::write(const char *psz) {
  if (m_pfileStream != NULL) {
    m_pfileStream->write(psz, (std::streamsize)strlen(psz));
  }
}

void CalvinToTextFile::writeLine(const AffxString& str) 
{
  std::ostringstream line;
  line << str << std::endl;
  write(line.str().c_str());
}



/** 
 * Error handling for the class.
 * @param strMessage - The error messagae to report.
 */
void CalvinToText::error(const AffxString& strMessage)
{
	Err::errAbort(strMessage);
}

void CalvinToText::run(const std::string& strFileNameIn,
                       const std::string& strFileNameOut,
                       bool bHeader,
                       bool bBody, 
                       bool bNewGuid)
{
  try 	{
    m_strFileNameIn = Fs::convertToUncPath(strFileNameIn);
    m_strFileNameOut = Fs::convertToUncPath(strFileNameOut);

    if (isCalvinFile(m_strFileNameIn)) {		
      AffxByteArray chpFileName;
      chpFileName.assign(m_strFileNameIn);
      Verbose::out(1, "Writing file: " + m_strFileNameOut);
      if (m_file.open(m_strFileNameOut) ){
        m_bHeader = bHeader;
        affymetrix_calvin_io::GenericData genericData;
        OutputHeader(m_strFileNameIn, genericData, bNewGuid);
        if (bBody){
            OutputGroupsAndSets(genericData);
        }
        m_file.close();
      }
      else 			{
        Err::errAbort("A problem occurred while processing file (Cannot open output file) " + m_strFileNameIn);
      }
    }
  } 

  //When things go wrong see if we can die gracefully here. 
  catch(Except &e) {
    Verbose::out(0,"");
    error("Exception caught. "
          "Message is: " + ToStr(e.what()));
  }
  catch(const std::bad_alloc &e) {
    Verbose::out(0,"");
    error("std::bad_alloc caught. "
          "The application has run out of memory, or the calvin file is malformed."
          "Message is: " + ToStr(e.what()));
  }
  catch(affymetrix_calvin_exceptions::CalvinException &ce) {
    Verbose::out(0,"");
    error("affymetrix_calvin_exceptions::CalvinException caught. "
          "Affymetrix GeneChip Command Console library has thrown an exception. "
          "Message is: " + affymetrix_calvin_utilities::StringUtils::ConvertWCSToMBS(ce.Description()));
  }
  catch(BroadException &e) {
    Verbose::out(0,"");
    error("BroadException caught. "
          "Message is: '" + ToStr(e.m_msg) + "' source file: '" + ToStr(e.m_sourcefile) +
          ":" + ToStr(e.m_sourceline) + "'");
  }
  catch(const std::exception &e) {
    Verbose::out(0,"");
    error("std::exception caught. "
          "Message is: " + ToStr(e.what()));
  }
  catch(...) {
    Verbose::out(0,"");
    error("Unknown exception caught. "
          "No message is available.");
  }
}

/*
 * Read the file into a data object.
 */
bool CalvinToText::ReadFile(const string &fileName, GenericData &gdata)
{
	GenericFileReader reader;

	// Read the file.
	reader.SetFilename(fileName);
	try
	{
		reader.ReadHeader(gdata);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/*
 * Output the data to the command line.
 */
void CalvinToText::OutputData(GenericData& gdata, int groupIndex, int setIndex, int startRow, int endRow)
{
	DataSet *set = gdata.DataSet(groupIndex, setIndex);
	set->Open();

	// Output the rows of data
	int8_t i8;
	u_int8_t iu8;
	int16_t i16;
	u_int16_t iu16;
	int32_t i32;
	u_int32_t iu32;
	float f32;
	std::string s;
	std::wstring ws;
	int nRows = set->Rows();
	int nCols = set->Cols();
	startRow = Min(Max(startRow, 0), nRows-1);
	if (startRow < 0) {startRow = 0;}
	if (endRow < 0) {endRow = nRows-1;}
	endRow = Min(endRow, nRows-1);
	AffxString strColumnName;
	int iDetectionColumn = -1;
	int iGenotypeCallColumn = -1;
	int iForcedGenotypeCallColumn = -1;
	int iChromosomeColumn = -1;
	if (nCols > 0)
	{
		for (int col=0; col<nCols; col++)
		{
			if (col != 0) {m_file.write("\t");}
			strColumnName = StringUtils::ConvertWCSToMBS(set->Header().GetColumnInfo(col).GetName()).c_str();
			if (strColumnName == "Detection") {iDetectionColumn = col;}
			else if (strColumnName == "Call") {iGenotypeCallColumn = col;}
			else if (strColumnName == "Forced Call") {iForcedGenotypeCallColumn = col;}
			else if (strColumnName == "Chromosome") {iChromosomeColumn = col;}
			m_file.write(strColumnName.c_str());
		}
		m_file.writeLine("");
	}
	for (int row=startRow; row<=endRow; row++)
	{
		for (int col=0; col<nCols; col++)
		{
			if (col != 0) {m_file.write("\t");}
			switch (set->Header().GetColumnInfo(col).GetColumnType())
			{
			case ByteColType:
				set->GetData(row, col, i8);
				m_file.write(::getInt((int)i8).c_str());
				break;
			case UByteColType:
				set->GetData(row, col, iu8);
				if (col == iDetectionColumn) 
				{
					switch(iu8) 
					{
					case 0: m_file.write("P"); break;
					case 1: m_file.write("M"); break;
					case 2: m_file.write("A"); break;
					case 3: m_file.write("N"); break;
					default: m_file.write(::getInt((int)iu8).c_str());
					}
				}
				else if ((col == iGenotypeCallColumn) || (col == iForcedGenotypeCallColumn)) 
				{
					switch(iu8) 
					{
					case 6: m_file.write("AA"); break;
					case 8: m_file.write("AB"); break;
					case 7: m_file.write("BB"); break;
					case 11: m_file.write("NC"); break;
					default: m_file.write(::getInt((int)iu8).c_str());
					}
				}
				else if (col == iChromosomeColumn)
				{
					if ((m_iXChromosome != -1) && (m_iXChromosome == iu8)) {m_file.write("X");}
					else if ((m_iXChromosome != -1) && (m_iYChromosome == iu8)) {m_file.write("Y");}
					else  {m_file.write(::getInt((int)iu8).c_str());}
				}
				else {m_file.write(::getInt((int)iu8).c_str());}
				break;
			case ShortColType:
				set->GetData(row, col, i16);
				m_file.write(::getInt(i16).c_str());
				break;
			case UShortColType:
				set->GetData(row, col, iu16);
				m_file.write(::getInt(iu16).c_str());
				break;
			case IntColType:
				set->GetData(row, col, i32);
				m_file.write(::getInt(i32).c_str());
				break;
			case UIntColType:
				set->GetData(row, col, iu32);
				m_file.write(::getUnsignedInt(iu32).c_str());
				break;
			case FloatColType:
				set->GetData(row, col, f32);
				m_file.write(::getDouble(f32, 6).c_str());
				break;
			case ASCIICharColType:
				set->GetData(row, col, s);
				m_file.write(s.c_str());
				break;
			case UnicodeCharColType:
				set->GetData(row, col, ws);
				m_file.write(StringUtils::ConvertWCSToMBS(ws).c_str());
				break;
			default:
				break;
			}
		}
		m_file.writeLine("");
	}
	set->Close();
	set->Delete();
}

/*
 * Output the file header parameters.
 */
void CalvinToText::OutputFileHeaderParameters(GenericData &gdata, bool bNewGuid)
{
//	cout << "#File Header Information" << endl;

	if (m_bHeader)
	{
		if (bNewGuid)
		{
			m_file.writeLine("#%File=" + m_strFileNameOut);
			m_file.writeLine("#%FileCreationTime=" + StringUtils::ConvertWCSToMBS(DateTime::GetCurrentDateTime().ToString()));
		}
		else
		{
			m_file.writeLine("#%File=" + m_strFileNameIn);
			m_file.writeLine("#%FileSize=" + ::getUnsignedInt(Fs::fileSize(m_strFileNameIn)));
			if (gdata.Header().GetGenericDataHdr()->GetFileCreationTime().length() > 0)
			{
				m_file.writeLine("#%FileCreationTime=" + StringUtils::ConvertWCSToMBS(gdata.Header().GetGenericDataHdr()->GetFileCreationTime()));
			}
			// Output the magic and version numbers.
			m_file.writeLine("#%Magic=" + ::getInt((int)gdata.Header().GetMagicNumber()));
			m_file.writeLine("#%Version=" + ::getInt((int)gdata.Header().GetVersion()));
		}		
	}
}

/*
 * Output the header parameters.
 */
void CalvinToText::OutputParameters(ParameterNameValueTypeIt &begin, ParameterNameValueTypeIt &end)
{
	ParameterNameValueType param;
	ParameterNameValueTypeIt it;
	for (it=begin; it!=end; ++it)
	{
		param = *it;
		if (m_bHeader)
		{
			m_file.writeLine("#%" + StringUtils::ConvertWCSToMBS(param.GetName()) + "=" + StringUtils::ConvertWCSToMBS(param.ToString()));
		}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-xChromosome") {m_iXChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-yChromosome") {m_iYChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-option-xChromosome") {m_iXChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-option-yChromosome") {m_iYChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}
	}
}

/*
 * Output the header parameters.
 */
void CalvinToText::OutputParameters(ParameterNameValueTypeConstIt &begin, ParameterNameValueTypeConstIt &end)
{
	ParameterNameValueType param;
	ParameterNameValueTypeConstIt it;
	for (it=begin; it!=end; ++it)
	{
		param = *it;
		if (m_bHeader)
		{
			m_file.writeLine("#%" + StringUtils::ConvertWCSToMBS(param.GetName()) + "=" +StringUtils::ConvertWCSToMBS(param.ToString()));
		}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-xChromosome") {m_iXChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-yChromosome") {m_iYChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-option-xChromosome") {m_iXChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}
		if (StringUtils::ConvertWCSToMBS(param.GetName()) == "affymetrix-algorithm-param-option-yChromosome") {m_iYChromosome = ::getInt(StringUtils::ConvertWCSToMBS(param.ToString()));}

	}
}

/*
 * Output the data header parameters.
 */
void CalvinToText::OutputDataHeaderParameters(GenericDataHeader *hdr, bool bNewGuid)
{
//	cout << "#Data Header Information" << endl;

	if (m_bHeader)
	{
		// Add the file id's.
		if (hdr->GetFileId().length() > 0)
		{
			if (bNewGuid)
			{
				m_file.writeLine("#%FileIdentifier=" + affxutil::Guid::GenerateNewGuid());
			}
			else
			{
				m_file.writeLine("#%FileIdentifier=" + hdr->GetFileId());
			}
		}
		if (hdr->GetFileTypeId().length() > 0)
		{
			m_file.writeLine("#%FileTypeIdentifier=" + hdr->GetFileTypeId());
		}
		if (hdr->GetLocale().length() > 0)
		{
			m_file.writeLine("#%FileLocale=" + StringUtils::ConvertWCSToMBS(hdr->GetLocale()));
		}
	}


	// Add the parameters.
	ParameterNameValueTypeIt begin;
	ParameterNameValueTypeIt end;
	hdr->GetNameValIterators(begin, end);
	OutputParameters(begin, end);

	int nParents = hdr->GetParentCnt();
	for (int iParent=0; iParent<nParents; iParent++)
	{
    // dont take address of temp
		GenericDataHeader parent=hdr->GetParent(iParent);
		OutputDataHeaderParameters(&parent, false);
	}
}

/*
 * Output the set information, only the header.
 */
void CalvinToText::OutputSetInfo(GenericData &gdata, int groupIndex, int setIndex)
{
	DataSet *pSet = gdata.DataSet(groupIndex, setIndex);
//	if (!g_bNoHeader)
	{
		m_file.writeLine("#%SetName=" + StringUtils::ConvertWCSToMBS(pSet->Header().GetName()));

		// Add the row and column count.
		m_file.writeLine("#%Columns=" + ::getInt(pSet->Cols()));
		m_file.writeLine("#%Rows=" + ::getInt(pSet->Rows()));
	}

	// Add the parameters.
	ParameterNameValueTypeConstIt begin;
	ParameterNameValueTypeConstIt end;
	pSet->Header().GetNameValIterators(begin, end);
	OutputParameters(begin, end);
	OutputData(gdata, groupIndex, setIndex, 0, -1);
	pSet->Delete();
}

/*
 * Output the group information.
 */
void CalvinToText::OutputGroupInfo(GenericData &gdata, const std::wstring &groupName, int groupIndex)
{
//	if (!g_bNoHeader)
	{
		m_file.writeLine("#%GroupName=" + StringUtils::ConvertWCSToMBS(groupName));
	}

	// Add each set.
	int iSetCount = gdata.DataSetCnt(groupIndex);
	for (int i=0; i<iSetCount; i++)
	{
		OutputSetInfo(gdata, groupIndex, i);
	}
}

/*
 * Output the groups and sets.
 */
void CalvinToText::OutputGroupsAndSets(GenericData &gdata)
{
	// Add each group.
	WStringVector groupNames;
	gdata.DataGroupNames(groupNames);
	int iGroupCount = (int)groupNames.size();
	for (int i=0; i< iGroupCount; i++)
	{
		OutputGroupInfo(gdata, groupNames[i], i);
	}
}

/*
 * Output the header information.
 */
void CalvinToText::OutputHeader(const string &strFileName, affymetrix_calvin_io::GenericData& genericData, bool bNewGuid)
{
	affymetrix_calvin_io::GenericFileReader reader;
	reader.SetFilename(strFileName);
	reader.Open(genericData);

	OutputFileHeaderParameters(genericData, bNewGuid);
	OutputDataHeaderParameters(genericData.Header().GetGenericDataHdr(), bNewGuid);
}

bool CalvinToText::isCalvinFile(const AffxString& strFileName)
{	
	AffxBinaryFile file;
	if (!file.open(strFileName, AffxBinaryFile::LOAD)) {return false;}
	unsigned char ucFileType = file.readUnsignedChar(); 
	file.close();
	if (ucFileType == 59) {return true;}
	else {return false;}
}






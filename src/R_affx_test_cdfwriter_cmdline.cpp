/********************************************

A command line writing of cel file.
Used for testing.

*********************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "CDFFileWriter.h"

using namespace std;
using namespace affymetrix_calvin_io;


void writeExpressionCDFFile(const std::string& filename, u_int32_t probeSetCnt)
{
	wchar_t name[100];

	u_int8_t unitType = 3;
	u_int8_t direction = 1;
	u_int32_t atoms = 11;
	u_int8_t cellsPerAtom = 2;
	u_int32_t cells = atoms*cellsPerAtom;
 
	CDFData data(filename);
	data.SetProbeSetCnt(probeSetCnt, Expression);
	data.SetArrayCols(atoms);
	data.SetArrayRows(probeSetCnt*cellsPerAtom);
	CDFFileWriter writer(data);

	for (u_int32_t i = 0; i < probeSetCnt; ++i)
	{
		// make the unit name
                // FormatString1(name, 100, L"biob_%d", i);

		writer.OpenDataGroup(L"tmp", 1);
		CDFProbeSetWriter* probeWriter = writer.CreateProbeSetWriter(name,unitType,direction,atoms,cells,i,cellsPerAtom);
		probeWriter->WriteHeader();

		for (u_int32_t atom = 0; atom < atoms; ++atom)
		{
			for (u_int32_t cellInAtom = 0; cellInAtom < cellsPerAtom; ++cellInAtom)
			{
				probeWriter->Write(atom,i*cellsPerAtom+cellInAtom,atom,atom,'C','G');
			}
		}
		probeWriter->Close();
		delete probeWriter;
		writer.CloseDataGroup();	
	}
}


int main(int argc, char **argv)
{
    // writeExpressionCDFFile("testExp.cdf", 5);

    const char* cdfFileName = argv[1];
    u_int32_t nProbeSets= 2;
    u_int32_t nCols = 10, nRows = 10;

    CDFData data(cdfFileName);
    data.SetProbeSetCnt(nProbeSets, Expression);
    data.SetArrayRows(nRows);
    data.SetArrayCols(nCols);
    CDFFileWriter* writer = new CDFFileWriter(data);

    u_int8_t unitType, direction, cellsPerAtom;
    u_int32_t atoms, cells;
    CDFProbeSetWriter* probeWriter;
//     const wstring probeSetName, groupSetName;
//     probeSetName = "affy probe set 1";




    writer->OpenDataGroup(L"affy probe set 1", 1);
    unitType = 3;
    direction = 1;
    atoms = 2;
    cellsPerAtom = 2;
    cells = atoms*cellsPerAtom;
    probeWriter = 
        writer->CreateProbeSetWriter(L"xda block name 1", unitType,
                                     direction, atoms, cells, 0,cellsPerAtom); 
//     (const std::wstring& xdaBlockName, 
    probeWriter->WriteHeader();
    probeWriter->Write(10,10,1,3,'C','A');
    probeWriter->Write(10,11,1,3,'C','A');
    probeWriter->Write(11,10,2,4,'C','A');
    probeWriter->Write(11,11,2,4,'C','A');
//        (u_int16_t xCoord,
//         u_int16_t yCoord,
//         u_int32_t atom,
//         u_int32_t indexPos,
//         int8_t baseProbe,
//         int8_t baseTarget);

    probeWriter->Close();
    delete probeWriter;
    writer->CloseDataGroup();
    
    writer->OpenDataGroup(L"affy probe set 2", 2);
    unitType = 3;
    direction = 1;
    atoms = 2;
    cellsPerAtom = 2;
    cells = atoms*cellsPerAtom;
    probeWriter = 
        writer->CreateProbeSetWriter(L"xda block name 1", unitType,
                                     direction, atoms, cells, 0,cellsPerAtom); 
    probeWriter->WriteHeader();
    probeWriter->Write(12,12,1,3,'C','A');
    probeWriter->Write(12,13,1,3,'C','A');
    probeWriter->Write(13,13,2,4,'C','A');
    probeWriter->Write(13,12,2,4,'C','A');
    probeWriter->Close();
    delete probeWriter;

    unitType = 3;
    direction = 1;
    atoms = 2;
    cellsPerAtom = 2;
    cells = atoms*cellsPerAtom;
    probeWriter = 
        writer->CreateProbeSetWriter(L"xda block name 2", unitType,
                                     direction, atoms, cells, 0,cellsPerAtom); 
    probeWriter->WriteHeader();
    probeWriter->Write(12,12,1,3,'C','A');
    probeWriter->Write(12,13,1,3,'C','A');
    probeWriter->Write(13,13,2,4,'C','A');
    probeWriter->Write(13,12,2,4,'C','A');
    probeWriter->Close();
    delete probeWriter;

    writer->CloseDataGroup();
    
    delete writer;
    return 0;
}




// ReadAffxFiles.cpp : Defines the entry point for the console application.
//

#include "CELFileData.h"
#include "CDFFileData.h"
#include "CHPFileData.h"
#include "BARFileData.h"
#include "BPMAPFileData.h"

using namespace affxcdf;
using namespace affxcel;
using namespace affxchp;
using namespace affxbar;
using namespace affxbpmap;

#include <iostream>
#include <stdio.h>

using namespace std;

void test_bar(string file)
{
	affxbar::TagValuePairType param;
	CBARFileData bar;
	bar.SetFileName(file.c_str());
	if (bar.Exists())
	{
		bar.Read();

		cout << "Version = " << bar.GetVersion() << endl;
		cout << "#Seqs = " << bar.GetNumberSequences() << endl;
		cout << "#Cols = " << bar.GetNumberColumns() << endl;
		cout << "#Params = " << bar.GetNumberParameters() << endl;

		for (int iparam=0; iparam<bar.GetNumberParameters(); iparam++)
		{
			param = bar.GetParameter(iparam);
			cout << param.Tag << " = " << param.Value << endl;
		}

		for (int iseq=0; iseq<bar.GetNumberSequences(); iseq++)
		{
			CGDACSequenceResultItem res;
			bar.GetResults(iseq, res);

			cout << "Name = " << res.GetName() << endl;
			cout << "Group = " << res.GetGroupName() << endl;
			cout << "Version = " << res.GetVersion() << endl;
			cout << "#Pts = " << res.GetNumberDataPoints() << endl;
			cout << "#Cols = " << res.GetNumberColumns() << endl;

			for (int iseqparam=0; iseqparam<res.GetNumberParameters(); iseqparam++)
			{
				affxbar::TagValuePairType &param = res.GetParameter(iseqparam);
				cout << param.Tag << "=" << param.Value << endl;
			}

			BarSequenceResultData data;
			for (int ipt=0; ipt<res.GetNumberDataPoints(); ipt++)
			{
				for (int icol=0; icol<res.GetNumberColumns(); icol++)
				{
					res.GetData(ipt, icol, data);

					switch (res.GetColumnType(icol))
					{
					case BAR_DATA_INTEGER:
						cout << data.iValue << endl;
						break;

					case BAR_DATA_FLOAT:
						cout << data.fValue << endl;
						break;
					}
				}
			}
		}
	}
}

void test_bpmap(string file)
{
	GDACSequenceHitItemType hit;
	affxbpmap::TagValuePairType param;
	CGDACSequenceItem seq;
	CBPMAPFileData bpmap;
	bpmap.SetFileName(file.c_str());
	if (bpmap.Exists())
	{
		bpmap.Read();

		cout << "#Seqs = " << bpmap.GetNumberSequences() << endl;
		cout << "Version = " << bpmap.GetVersion() << endl << endl;

		for (int iseq=0; iseq<bpmap.GetNumberSequences(); iseq++)
		{
			bpmap.GetSequenceItem(iseq, seq);
			cout << "Name = " << seq.GetName() << endl;
			cout << "Group = " << seq.GroupName() << endl;
			cout << "Version = " << seq.GetSeqVersion() << endl;
			cout << "Number = " << seq.GetNumber() << endl;
			cout << "#Hits = " << seq.GetNumberHits() << endl;
			cout << "#Params = " << seq.GetNumberParameters() << endl;

			for (int iparam=0; iparam<seq.GetNumberParameters(); iparam++)
			{
				param = seq.GetParameter(iparam);
				cout << param.Tag << " = " << param.Value << endl;
			}

			for (int ihit=0; ihit<seq.GetNumberHits(); ihit++)
			{
				seq.GetHitItem(ihit, hit, true);

				cout
					<< hit.PMX << "\t"
					<< hit.PMY << "\t"
					<< hit.MMX << "\t"
					<< hit.MMY << "\t"
					<< hit.MatchScore << "\t"
					<< hit.Position << "\t"
					<< hit.PMProbe << "\t"
					<< (int) hit.ProbeLength << "\t"
					<< (int) hit.TopStrand << endl;

			}	
		}
	}
}

void test_cel(string file)
{
	CCELFileData cel;
	CELFileEntryType entry;
	cel.SetFileName(file.c_str());

	cout << "entry size = " << sizeof(CELFileEntryType) << endl << endl;

	if (cel.Exists())
	{
		cel.Read();
		CCELFileHeaderData &header = cel.GetHeader();
		cout << "Version = " << header.GetVersion() << endl;
		cout << "#Cols = " << header.GetCols() << endl;
		cout << "#Rows = " << header.GetRows() << endl;
		cout << "#Total = " << header.GetNumCells() << endl;
		cout << "Header = " << header.GetHeader() << endl;
		cout << "Alg = " << header.GetAlg() << endl;
		cout << "Params = " << header.GetParams() << endl;
		cout << "Array type = " << header.GetChipType() << endl;
		cout << "Margin = " << header.GetCellMargin() << endl;
		cout << "#Outliers = " << header.GetNumOutliers() << endl;
		cout << "#Masked = " << header.GetNumMasked() << endl;

		GridCoordinatesType &corners = header.GridCorners();
		cout << "Corners = "
			<< corners.upperleft.x << "\t"
			<< corners.upperleft.y << "\t"

			<< corners.upperright.x << "\t"
			<< corners.upperright.y << "\t"

			<< corners.lowerright.x << "\t"
			<< corners.lowerright.y << "\t"

			<< corners.lowerleft.x << "\t"
			<< corners.lowerleft.y << endl;


		for (int icel=0; icel<header.GetNumCells(); icel++)
		{
			cel.GetEntry(icel, entry);
			cout 
				<< cel.IndexToX(icel) << "\t"
				<< cel.IndexToY(icel) << "\t"
				<< entry.Intensity << "\t"
				<< entry.Stdv << "\t"
				<< entry.Pixels << "\t"
				<< cel.IsOutlier(icel) << "\t"
				<< cel.IsMasked(icel) << endl;
		}

		for (int ix=0; ix<header.GetCols(); ix++)
		{
			for (int iy=0; iy<header.GetRows(); iy++)
			{
				cel.GetEntry(ix, iy, entry);
				cout 
					<< ix << "\t"
					<< iy << "\t"
					<< entry.Intensity << "\t"
					<< entry.Stdv << "\t"
					<< entry.Pixels << "\t"
					<< cel.IsOutlier(ix, iy) << "\t"
					<< cel.IsMasked(ix, iy) << endl;
			}
		}

		int noutlier=0;
		int nmasked=0;
		for (int icel=0; icel<header.GetNumCells(); icel++)
		{
			if (cel.IsOutlier(icel) == true)
				++noutlier;

			if (cel.IsMasked(icel) == true)
				++nmasked;
		}

		cel.Close();
	}
}

void test_chp(string file)
{
	CExpressionProbeSetResults *exp;
	CGenotypeProbeSetResults *geno;
	CCHPFileHeader header;
	CCHPFileData chp;
	chp.SetFileName(file.c_str());
	if (chp.Exists())
	{
		chp.Read();

		header = chp.GetHeader();

		cout << "Cols = " << header.GetCols() << endl;
		cout << "Rows = " << header.GetRows() << endl;
		cout << "Sets = " << header.GetNumProbeSets() << endl;
		cout << "Array type = " << header.GetChipType() << endl;
		cout << "Alg = " << header.GetAlgName() << endl;
		cout << "Version = " << header.GetAlgVersion() << endl;
		cout << "CEL file = " << header.GetParentCellFile() << endl;
		cout << "ProgID = " << header.GetProgID() << endl;
		cout << "Alpha1 = " << header.GetAlgorithmParameter("Alpha1") << endl;
		cout << "RawQ = " << header.GetSummaryParameter("RawQ") << endl;

		affxchp::TagValuePairType param;
		affxchp::TagValuePairTypeList::iterator iter;
		for (iter=header.AlgorithmParameters().begin(); iter!=header.AlgorithmParameters().end(); ++iter)
		{
			cout << (*iter).Tag << " = " << (*iter).Value << endl;
		}
		for (iter=header.SummaryParameters().begin(); iter!=header.SummaryParameters().end(); ++iter)
		{
			cout << (*iter).Tag << " = " << (*iter).Value << endl;
		}

		for (int ips=0; ips<header.GetNumProbeSets(); ips++)
		{
			if (header.GetAssayType() == CCHPFileHeader::Expression)
			{
				exp = chp.GetExpressionResults(ips);
				cout
					<< exp->Signal << "\t"
					<< exp->DetectionPValue << "\t"
					<< exp->Signal << "\t" 
					<< exp->NumPairs << "\t"
					<< exp->NumUsedPairs << "\t"
					<< (int) exp->Detection << "\t"
					<< exp->GetDetectionString();

				if (exp->m_HasCompResults)
				{
					cout << "\t"
						<< exp->m_HasCompResults << "\t"
						<< exp->ChangePValue << "\t" 
						<< exp->SignalLogRatio << "\t" 
						<< exp->SignalLogRatioLow << "\t" 
						<< exp->SignalLogRatioHigh << "\t" 
						<< exp->NumCommonPairs << "\t"
						<< (int) exp->Change << "\t"
						<< exp->GetChangeString();
				}
				cout << endl;
			}
			else if (header.GetAssayType() == CCHPFileHeader::Genotyping)
			{
				geno = chp.GetGenotypingResults(ips);
				cout
					<< (int) geno->AlleleCall << "\t"
					<< geno->Confidence << "\t"
					<< geno->RAS1 << "\t"
					<< geno->RAS2 << "\t"
					<< geno->pvalue_AA << "\t"
					<< geno->pvalue_AB << "\t"
					<< geno->pvalue_BB << "\t"
					<< geno->pvalue_NoCall << "\t"
					<< geno->GetAlleleCallString() << endl;
			}
		}
	}
}

void test_cdf(string file)
{
	CCDFProbeInformation cel;
	CCDFProbeSetInformation set;
	CCDFProbeGroupInformation group;
	CCDFFileHeader header;
	CCDFFileData cdf;
	cdf.SetFileName(file.c_str());
	if (cdf.Exists())
	{
		cdf.Read();

		cout << "Array type = " << cdf.GetChipType() << endl;

		header = cdf.GetHeader();
		for (int ips=0; ips<header.GetNumProbeSets(); ips++)
		{
			string name = cdf.GetProbeSetName(ips);
			cout << "Name" << ips << " = " << name << endl;
		}

		CCDFQCProbeSetInformation qcunit;
		CCDFQCProbeSetInformation qcunit2;
		CCDFQCProbeInformation qcprobe;
		for (int ips=0; ips<header.GetNumQCProbeSets(); ips++)
		{
			cdf.GetQCProbeSetInformation(ips, qcunit);
			cout << "Type = " << qcunit.GetQCProbeSetType() << endl;
			cout << "#Cells = " << qcunit.GetNumCells() << endl;
			for (int icel=0; icel<qcunit.GetNumCells(); icel++)
			{
				qcunit.GetProbeInformation(icel, qcprobe);
				cout << qcprobe.GetX() << "\t" << qcprobe.GetY() << "\t" << qcprobe.GetPLen() << "\t" << endl;
			}


			cdf.GetQCProbeSetInformation(qcunit.GetQCProbeSetType(), qcunit2);
			if (qcunit2.GetNumCells() == qcunit.GetNumCells() && qcunit.GetQCProbeSetType() == qcunit2.GetQCProbeSetType())
				cout << "Get by type passed" << endl;
			else
				cout << "Get by type failed" << endl;
		}

		for (int ips=0; ips<header.GetNumProbeSets(); ips++)
		{
			string name = cdf.GetProbeSetName(ips);
			cout << endl << "Probe set #" << ips+1 << endl;
			cout << "Name" << " = " << name << endl;
			cout << "Type = " << cdf.GetProbeSetType(ips) << endl;
			cdf.GetProbeSetInformation(ips, set);

			cout
				<< endl
				<< "#lists = " << set.GetNumLists() << endl
				<< "#groups = " << set.GetNumGroups() << endl
				<< "#cells = " << set.GetNumCells() << endl
				<< "#cellsperlist = " << set.GetNumCellsPerList() << endl
				<< "Number = " << set.GetProbeSetNumber() << endl
				<< "Type = " << set.GetProbeSetType() << endl
				<< "Dir = " << set.GetDirection() << endl;

			int n = set.GetNumGroups();
			for (int ig=0; ig<n; ig++)
			{
				set.GetGroupInformation(ig, group);

				cout
					<< endl
					<< "#lists = " << group.GetNumLists() << endl
					<< "#cells = " << group.GetNumCells() << endl
					<< "#cellsperlist = " << group.GetNumCellsPerList() << endl
					<< "start = " << group.GetStart() << endl
					<< "stop = " << group.GetStop() << endl
					<< "Name = " << group.GetName() << endl;

				for (int ic=0; ic<group.GetNumCells(); ic++)
				{
					group.GetCell(ic, cel);

					cout
						<< ic << " =\t"
						<< cel.GetX() << "\t"
						<< cel.GetY() << "\t"
						<< cel.GetListIndex() << "\t"
						<< cel.GetExpos() << "\t"
						<< cel.GetPBase() << "\t"
						<< cel.GetTBase() << endl;
				}
			}
		}
	}
}


void test_file_readers(std::string file)
{
	// The input file is assumed to be a full path file name.

	if (strstr(file.c_str(), ".cel") != NULL || strstr(file.c_str(), ".CEL") != NULL)
	{
		test_cel(file);
	}

	else if (strstr(file.c_str(), ".cdf") != NULL || strstr(file.c_str(), ".CDF") != NULL)
	{
		test_cdf(file);
	}

	else if (strstr(file.c_str(), ".chp") != NULL || strstr(file.c_str(), ".CHP") != NULL)
	{
		test_chp(file);
	}

	else if (strstr(file.c_str(), ".bar") != NULL || strstr(file.c_str(), ".BAR") != NULL)
	{
		test_bar(file);
	}

	else if (strstr(file.c_str(), ".bpmap") != NULL || strstr(file.c_str(), ".BPMAP") != NULL)
	{
		test_bpmap(file);
	}
}


/** JHB added for fun. **/
int main(int argc, char* argv[])
{
  std::string fname; 
  cout << "In main, calling with nargs: " << argc << endl;

  if (argc >= 2) {
    fname = argv[1];
    cout << "filename: " << fname << endl;
  }
  else {
    cout << "usage: " << argv[0] << " filename " << endl;
    return 1; 
  }
  
  test_file_readers(fname);
}

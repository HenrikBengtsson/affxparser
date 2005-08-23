#include "FusionCELData.h"
#include "FusionCDFData.h"
#include <iostream>

using namespace std;
using namespace affymetrix_fusion_io;

int main(int argc, char **argv)
{
  const char* celFileName = argv[1];
  const char* cdfFileName = argv[2];
  FusionCELData cel;
  FusionCDFData cdf;
  try
    {
      cel.SetFileName(celFileName);
      if (cel.Read() == false)
	{
	  cout << "Failed to read the file." << endl;
	  return 0;
	}
      int n = cel.GetNumCells();
      float sum = 0;
      for (int i = 0; i < n; i++)
	{
	  sum += cel.GetIntensity(i);
	}
      float avg = sum / n;
      cout << "The average intensity is: " << avg << endl;

      cdf.SetFileName(cdfFileName);
      if (cdf.Read() == false)
	{
	  cout << "Failed to read the CDF file." << endl;
	  return 0;
	}
        
      int nsets = cdf.GetHeader().GetNumProbeSets();
      std::string name;
      for (int iset=0; iset<nsets; iset++)
	{
	  name = cdf.GetProbeSetName(iset);
	  sum = 0;
	  FusionCDFProbeSetInformation set;
	  cdf.GetProbeSetInformation(iset, set);
	  int ngroups = set.GetNumGroups();
	  for (int igroup=0; igroup<ngroups; igroup++)
	    {
	      FusionCDFProbeGroupInformation group;
	      set.GetGroupInformation(igroup, group);
	      int ncells = group.GetNumCells();
	      for (int icell=0; icell<ncells; icell++)
		{
		  FusionCDFProbeInformation probe;
		  group.GetCell(icell, probe);
		  sum += cel.GetIntensity(probe.GetX(), probe.GetY());
		}
	    }
	  avg = sum / set.GetNumCells();
	  cout << "The average probe set intensity (" << name << ") is " << avg << endl;
	}
    }
  catch (...)
    {
      cout << "Error in reading the file.";
    }

  return 0;
}


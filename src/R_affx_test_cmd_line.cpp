/********************************************

A command line parsing of cel/cdf files.
Used for testing.

*********************************************/
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
      int n = (int) cel.GetNumCells();
      cout << "Only printing first 10 lines in CDF file." << endl;
      for (int i = 0; i < 10; i++)
	{
	    cout <<  
                "intensity: " << cel.GetIntensity(i) <<
                " x: " << cel.IndexToX(i) << " y: " << cel.IndexToY(i) <<
                " pixels: " << cel.GetPixels(i) << " stdv: " << 
                cel.GetStdv(i) << endl;

	  if (cel.IsOutlier(i) == true) {
	    cout << "Outlier at index: " << i << endl;
	  }
	
	  if (cel.IsMasked(i) == true) {
	    cout << "Masked at index: " << i << endl;
	  }
	  
	}

      if(argc > 2){
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
                  double sum = 0;
                  FusionCDFProbeSetInformation set;
                  cdf.GetProbeSetInformation(iset, set);
                  cout << "Unit type is " << set.GetProbeSetType() << endl;
                  if(set.GetProbeSetType() == affxcdf::ExpressionProbeSetType)
                      cout << "Exp" << endl;
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
                  double avg;
                  avg = sum / set.GetNumCells();
                  cout << "The average probe set intensity (" << name << ") is " << avg << endl;
              }
      }      
    }
  catch (std::exception& e)
      {
          cout << e.what() << endl;
          cout << "Error in reading the file.";
      }
  
  return 0;
}


/********************************************

A command line parsing of cel/cdf files.
Used for testing.

*********************************************/
#include "BPMAPFileData.h"
#include "FusionBPMAPData.h"
#include <iostream>

using namespace std;
using namespace affymetrix_fusion_io;

int main(int argc, char **argv)
{
  const char* bpmapFileName = argv[1];
  CBPMAPFileData bpmap;
  bpmap.SetFileName(bpmapFileName);
  if(bpmap.ReadHeader() == false)
      {
	  cout << "Failed to read the file." << endl;
	  return 0;
      }
  bpmap.Read();
  cout << "Reading " << bpmap.GetFileName() << endl;
  cout << "Number of sequences: " << bpmap.GetNumberSequences() << endl;
  cout << "Version: " << bpmap.GetVersion() << endl;
  int numSeq = bpmap.GetNumberSequences();
  affxbpmap::CGDACSequenceItem seq;
  for(int i = 0; i < numSeq ; i++)
      {
          bpmap.GetSequenceItem(i, seq);
          cout << "Sequence Name: " << seq.GetName() << endl;
          cout << "Sequence Group Name: " << seq.GroupName() << endl;
          cout << "Sequence Full Name: " << seq.FullName() << endl;
          cout << "Sequence Version: " << seq.GetSeqVersion() << endl;
          cout << "Sequence Probe Mapping: " << seq.GetProbeMapping() << endl;
          cout << "Sequence Number: " << seq.GetNumber() << endl;
          cout << "Sequence Number of Hits: " << seq.GetNumberHits() << endl;
          cout << "Sequence Number of Parameters: " << seq.GetNumberParameters() << endl;
          // seq.GetParameter
          int numPar = seq.GetNumberParameters();
          TagValuePairType tagPar;
          for(int j = 0; j < numPar; j++)
              {
                  tagPar = seq.GetParameter(j);
                  cout << "  Parameter Tag: " << tagPar.Tag << endl;
                  cout << "  Parameter Value: " << tagPar.Value << endl;
              }
          int numHits = seq.GetNumberHits();
          affxbpmap::GDACSequenceHitItemType seqHit;
          for(int j = 0; j < min(numHits, 2) ; j++)
              {
                  seq.GetHitItem(j, seqHit, true);
                  cout << "   Hit pmx: " << seqHit.PMX << endl;
                  cout << "   Hit pmy: " << seqHit.PMY << endl;
                  cout << "   Hit mmx: " << seqHit.MMX << endl;
                  cout << "   Hit mmy: " << seqHit.MMY << endl;
                  cout << "   Hit matchscore: " << seqHit.MatchScore << endl;
                  cout << "   Hit pmprobe seq: " << seqHit.PMProbe << endl;
                  cout << "   Hit pmprobe length: " << seqHit.ProbeLength + 0 << endl;
                  cout << "   Hit topstrand: " << seqHit.TopStrand + 0 << endl;
                  // cout << "   Hit pmprobe packed: " << seqHit.PackedPMProbe[0] << endl;
                  // could not get pmprobepacked to work
                  cout << "   Hit genomic center position: " << seqHit.getCenterPosition() << endl;
                  cout << "   Hit genomic start position: " << seqHit.getStartPosition() << endl;
              }
      }
  bpmap.Close();
  return 0;
}


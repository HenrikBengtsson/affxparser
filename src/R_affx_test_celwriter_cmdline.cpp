/********************************************

A command line writing of cel file.
Used for testing.

*********************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "CalvinCelFileWriter.h"
#include "CELData.h"

using namespace std;
using namespace affymetrix_calvin_io; // note calvin instead of fusion?

// void transformCels(Options &o) {
//   CelReader cReader;               // Object for getting data from cel files.
//   ChipLayout layout;               // Specifies probesets, locations of features on chip.
//   ChipStream *cStream = NULL;      // Our chipstream for transforming data.
//   vector<string> words;
//   vector<ChipStream *> cStreamVec;
//   affxcel::CCELFileWriter cel;
//   ChipStreamFactory cFactory;
//   if(o.targetSketch != "") 
//     cFactory.readTargetSketchFromFile(o.targetSketch.c_str());
//   if(o.writeSketch) 
//     cFactory.setWriteSketch(o.outDir + PATH_SEPARATOR);
//   if(o.probeNormFile != "") 
//     cFactory.readNormProbesFromFile(o.probeNormFile.c_str());

//   const char *cdfFile = o.cdfFile.c_str();
//   const char *chipStreamStr = o.chipstream.c_str();
//   const char *outDir = o.outDir.c_str();

//   if(o.cdfFile != "") {
//     /* Get the layout for the chip. */
//     cout << "Opening cdf file: " << cdfFile << endl;
//     layout.openCdfAll(cdfFile);  
//   }
//   else {
//     int xCount, yCount;
//     cout << "Setting layout dimensions from cel file. Assuming all probes are PM." << endl;
//     getDimensions(o.celFiles[0], xCount, yCount);
//     layout.setDimensions(xCount, yCount);
//     vector<bool> mask(layout.getProbeCount(), true);
//     layout.setPmProbeMask(mask);
//   }
//   /* Create our chipstream objects. */
//   Util::chopString(chipStreamStr, ',', words);
//   for(unsigned int i = 0; i < words.size(); i++) {
//     string dummy;
//     cStream = cFactory.chipStreamForString(words[i], layout, dummy );
//     if(!cStreamVec.empty())
//       cStreamVec[cStreamVec.size() - 1]->registerStream(cStream);
//     cStreamVec.push_back(cStream);

//   }
//   cReader.setFiles(o.celFiles);
//   /* Let reader know to pass data to our chipstream object. */
//   cReader.registerStream(cStreamVec[0]);
//   cout << "Reading cel files." << endl;
//   /* Open, read, and send cel file data one by one to chipstream. */
//   bool continueProcessing = true;
//   cReader.readFiles(&continueProcessing);

//   /* Transform the data and write it out. */
//   cout << "Adjusting and writing files";
//   for(int i = 0; i < o.celFiles.size(); i++) {
//     cout.put('.');
//     cout.flush();
//     cel.SetFileName(o.celFiles[i].c_str());
//     if(!cel.Read()) 
//       Err::errAbort("Can't read cel file: " + cel.GetFileName());
//     // the celfile might be mapped read-only -- make it writeable
//     cel.EnsureNotMmapped();
//     int numCells = cel.GetNumCells();
//     for(int celIx = 0; celIx < numCells; celIx++) {
//       float intensity = ChipStream::transformData(cel.GetIntensity(celIx), celIx, i, cStreamVec);
//       cel.SetIntensity(celIx, intensity);
//     }
//     string outCel = ToStr(outDir) + ToStr(PATH_SEPARATOR) + Util::fileRoot(cel.GetFileName());
//     cel.SetFileName(outCel.c_str());
//     if(!writeCelFile(cel, o.celFormat))
//       cout << "Warning could not save file: " << outCel << endl;
//     cel.Close();
//   }
//   cout << "Done." << endl;
// }


int main(int argc, char **argv)
{
  const char* celFileName = argv[1];
  CelFileData writerData(celFileName);
  writerData.SetIntensityCount(10);
  writerData.SetStdDevCount(10);
  writerData.SetPixelCount(10);
  writerData.SetOutlierCount(10);
  writerData.SetMaskCount(10);
  writerData.SetArrayType(L"arraytype");
  writerData.SetMasterFileName(L"masterfile");
  writerData.SetLibraryPackageName(L"libpackage");
  // look in data/src/CELData.h
  //   SetAlgorithmName
  // SetRows
  // SetCols
  // Stdev, numPixels, outliers and masked are optional
  // AddAlgorithmParameter

  
  CelFileWriter writer(writerData);

  FloatVector intensities;
  intensities.push_back(10.0);
  intensities.push_back(11.0);
  intensities.push_back(12.0);
  intensities.push_back(13.0);
  writer.WriteIntensities(intensities);

  FloatVector stdDevs;
  stdDevs.push_back(10.0);
  stdDevs.push_back(11.0);
  stdDevs.push_back(12.0);
  stdDevs.push_back(13.0);
  writer.WriteStdDevs(stdDevs);

  Int16Vector pixels;
  pixels.push_back(10);
  pixels.push_back(11);
  pixels.push_back(12);
  pixels.push_back(13);
  writer.WritePixels(pixels);

  return 0;
}


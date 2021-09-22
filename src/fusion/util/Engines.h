////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Affymetrix, Inc.
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
#include "canary/CanaryEngine.h"
#include "chipstream/apt-geno-qc/GenoQC.h"
#include "chipstream/apt-probeset-genotype/ProbesetGenotypeEngine.h"
#include "chipstream/apt-probeset-summarize/ProbesetSummarizeEngine.h"
#include "chipstream/apt-summary-genotype/SummaryGenotypeEngine.h"
#include "copynumber/CNAnalysisEngine.h"
#include "copynumber/CNCytoEngine.h"
#include "copynumber/CNFamilialEngine.h"
#include "copynumber/CNLog2RatioEngine.h"
#include "copynumber/CNReferenceEngine.h"
#include "copynumber/CNWorkflowEngine.h"
#include "dmet/DmetCHPWriter.h"
#include "dmet/DmetCopyNumberEngine.h"
#include "dmet/DmetEngine.h"
#include "mas5-stat/apt-mas5/MAS5Engine.h"
#include "translation/TranslationEngine.h"

namespace affx {

// This is basically a hack to work around the use of .a files
// In short, unless we explicitly pull in the engines, the static
// vars will not be pulled into the binaries and initialized
void touchEngines() {
    { CanaryEngine engine; }
    { GenoQC engine; }
    { ProbesetGenotypeEngine engine; }
    { ProbesetSummarizeEngine engine; }
    { SummaryGenotypeEngine engine; }
    { CNAnalysisEngine engine; }
    { CNCytoEngine engine; }
    { CNFamilialEngine engine; }
    { CNLog2RatioEngine engine; }
    { CNReferenceEngine engine; }
    { CNWorkflowEngine engine; }
    { DmetCHPWriter engine; }
    { DmetCopyNumberEngine engine; }
    { DmetEngine engine; }
    { MAS5Engine engine; }
    { TranslationEngine engine; }
}

}


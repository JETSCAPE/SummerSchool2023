/*******************************************************************************
 * Copyright (c) The JETSCAPE Collaboration, 2018
 *
 * Modular, task-based framework for simulating all aspects of heavy-ion collisions
 *
 * For the list of contributors see AUTHORS.
 *
 * Report issues at https://github.com/JETSCAPE/JETSCAPE/issues
 *
 * or via email to bugs.jetscape@gmail.com
 *
 * Distributed under the GNU General Public License 3.0 (GPLv3 or later).
 * See COPYING for details.
 ******************************************************************************/
// ---------------------------------------------
// ISR/small sytem Brick Test XSCAPE Framework
// ---------------------------------------------

#include <iostream>
#include <time.h>

// JetScape Framework includes ...
#include "JetScape.h"
#include "JetEnergyLoss.h"
#include "JetEnergyLossManager.h"
#include "HadronizationManager.h"
#include "Hadronization.h"
#include "IsrManager.h"
#include "IsrJet.h"
#include "IsrShowerPSG.h"

// Various output writer includes ...
#include "JetScapeWriterFinalStateStream.h"
#include "JetScapeWriterStream.h"
#include "JetScapeWriterIsrStream.h"
#ifdef USE_HEPMC
#include "JetScapeWriterHepMC.h"
#endif

// User modules derived from jetscape framework clasess ...
#include "Matter.h"
#include "Brick.h"
#include "PythiaIsrGun.h"
#include "iColoredHadronization.h"
#include "iMATTER.h"
#include "MCGlauberWrapper.h"
#include "MCGlauberGenStringWrapper.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace Jetscape;

// Forward declaration
void Show();

// -------------------------------------

int main(int argc, char** argv)
{
  clock_t t; t = clock();
  time_t start, end; time(&start);

  cout<<endl;

  // DEBUG=true by default and REMARK=false
  // can be also set also via XML file (at least partially)
  JetScapeLogger::Instance()->SetInfo(true);
  JetScapeLogger::Instance()->SetDebug(false);
  JetScapeLogger::Instance()->SetRemark(false);
  //SetVerboseLevel (9 a lot of additional debug output ...)
  //If you want to suppress it: use SetVerboseLevle(0) or max  SetVerboseLevle(9) or 10
  JetScapeLogger::Instance()->SetVerboseLevel(0);

  Show();

  auto jetscape = make_shared<JetScape>();

  // TAKING THE OUTPUT FOLDER FROM THE ARGS
  std::string mainXMLName = "../config/jetscape_main.xml";
  std::string userXMLName = "../config/jetscape_user_iMATTERMCGlauberMUSIC.xml";
  if (argc == 2)  {
    if ( strcmp(argv[1], "--help")==0 || strcmp(argv[1], "-h")==0 ){
      std::cout << "Command line options:" << std::endl;
      std::cout << "    First (optional) argument: path to user XML file         ./PythiaIsrTest /path/to/user.xml" << std::endl;
      std::cout << "    Second (optional) argument: path to main XML file      ./PythiaIsrTest /path/to/user.xml /path/to/main.xml" << std::endl;
      std::cout << "    If no command line options are given, defaults are used: config/jetscape_user.xml config/jetscape_main.xml" << std::endl;
      return -1;
    }
    else {
      userXMLName = argv[1];
    }
  }
  else if (argc == 3) {
    userXMLName = argv[1];
    mainXMLName = argv[2];
  }

  JSINFO << "Selecting main XML file: " << mainXMLName;
  JSINFO << "Selecting user XML file: " << userXMLName;

  jetscape->SetXMLMainFileName(mainXMLName);
  jetscape->SetXMLUserFileName(userXMLName);

  JetScapeXML::Instance()->OpenXMLMainFile(jetscape->GetXMLMainFileName());
  JetScapeXML::Instance()->OpenXMLUserFile(jetscape->GetXMLUserFileName());

  INFO_NICE;

  jetscape->SetId("primary");

  // Initial conditions and hydro
  auto MCG = make_shared<MCGlauberWrapper>();
  auto pythiaIsrGun= make_shared<PythiaIsrGun> ();
  auto hydro = make_shared<Brick> ();

  jetscape->Add(MCG);

  // ISR Mangers Shower module
  auto isrManager = make_shared<IsrManager>();
  auto isrJloss = make_shared<IsrJet>();
  auto isrPSG = make_shared<IsrShowerPSG>();
  // minor changes to allow backward time evolution (wrt to DoShower() in JetEnergyLoss class implementation)
  // and implementation of per timestep execution if needed in the future ...
  auto iMatter = make_shared<iMATTER> ();

  // Reading tMax from the xml
  double tMax = jetscape->GetXMLElementDouble({"Eloss", "maxT"});

  // iMatter showers negative virtuality partons from 0 => -tMax
  isrJloss->SetDeltaT(-0.1); isrJloss->SetStartT(0); isrJloss->SetMaxT(-tMax);
  iMatter->SetMaxT(-tMax);

  auto MCGsecond = make_shared<MCGlauberGenStringWrapper>();

  isrJloss->AddPartonShowerGenerator(isrPSG);
  isrJloss->Add(iMatter);
  isrManager->Add(isrJloss);

  pythiaIsrGun->Add(isrManager);
  pythiaIsrGun->Add(MCGsecond);
  jetscape->Add(pythiaIsrGun);
  jetscape->Add(hydro);

  // Energy loss
  // Matter showers positive virtuality partons in forward evolution, -tMax => tMax
  auto jlossmanager = make_shared<JetEnergyLossManager> ();
  auto jloss = make_shared<JetEnergyLoss> ();
  jloss->SetDeltaT(0.1); jloss->SetStartT(-tMax); jloss->SetMaxT(tMax);
  auto matter = make_shared<Matter> ();

  jloss->Add(matter);
  jlossmanager->Add(jloss);
  jetscape->Add(jlossmanager);

  // Hadronization Module which uses the colors of partons from ISR to FSR
  auto hadroMgr = make_shared<HadronizationManager> ();
  auto hadro = make_shared<Hadronization> ();
  auto hadroModule = make_shared<iColoredHadronization> ();
  hadro->Add(hadroModule);
  hadroMgr->Add(hadro);
  jetscape->Add(hadroMgr);

  // Output writer and filename setup from command line ...
  std::string outputFilename = jetscape->GetXMLElementText({"outputFilename"});

  auto writer = make_shared<JetScapeWriterFinalStatePartonsAscii>();
  auto writer2 = make_shared<JetScapeWriterFinalStateHadronsAscii>();
  writer->SetOutputFileName(outputFilename + string("_final_state_partons.dat"));
  writer2->SetOutputFileName(outputFilename + string("_final_state_hadrons.dat"));
  writer->SetId("FinalStatePartonsAscii"); //for task search test ...
  writer2->SetId("FinalStateHadronsAscii"); //for task search test ...
  jetscape->Add(writer);
  jetscape->Add(writer2);

  auto writerIsr= make_shared<JetScapeWriterIsrAscii> (outputFilename + "_isr.dat");
  writerIsr->SetId("IsrAsciiWriter"); //for task search test ...
  jetscape->Add(writerIsr);

  // Additional output writer examples ...
  // For HepmC output in ROOT format see PythiaBrickTestRoot.cc custom example.
  /*
  auto writerStd= make_shared<JetScapeWriterAscii> (outputFilename + ".dat");
  writerStd->SetId("StdAsciiWriter"); //for task search test ...
  jetscape->Add(writerStd);

  #ifdef USE_HEPMC
  auto writerHepMC= make_shared<JetScapeWriterHepMC> (outputFilename + ".hepmc");
  writerHepMC->SetId("hepMCWriter");
  jetscape->Add(writerHepMC);
  #endif
  */
  
  // Intialize all modules tasks
  jetscape->Init();

  // Run JetScape with all task/modules as specified
  jetscape->Exec();

  // For the future, cleanup is mostly already done in write and clear
  jetscape->Finish();

  INFO_NICE<<"Finished!";
  cout<<endl;

  t = clock() - t;
  time(&end);
  printf ("CPU time: %f seconds.\n",((float)t)/CLOCKS_PER_SEC);
  printf ("Real time: %f seconds.\n",difftime(end,start));

  return 0;
}

// -------------------------------------

void Show()
{
  INFO_NICE<<"-----------------------------------------------";
  INFO_NICE<<"| ISR/small sytem Brick Test XSCAPE Framework |";
  INFO_NICE<<"-----------------------------------------------";
  INFO_NICE;
}

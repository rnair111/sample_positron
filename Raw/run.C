#if !defined (__CINT__) || defined (__CLING__)
#include <string>
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliAODInputHandler.h"
#include "AliTender.h"
#include "AliAnalysisTaskMyCorrection.h"
#include "AliAnalysisGrid.h"
#endif
void SetupEnvironment();
void run(TString runMode = "full")
{
    const Bool_t local = kTRUE;
    const Bool_t isMC = kFALSE;
    const Int_t recoPass = 2;

    TGrid::Connect("alien://");
    SetupEnvironment();

    gROOT->LoadMacro( "AliAnalysisTaskMyCorrection.cxx+g" );
    AliAnalysisTaskMyCorrection* task = new AliAnalysisTaskMyCorrection();
    if(!task) return;

    TMacro physseladd(gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C"));
    AliPhysicsSelectionTask *physseltask = reinterpret_cast<AliPhysicsSelectionTask *>(physseladd.Exec(Form("%d", isMC)));
    task->SelectCollisionCandidates(AliVEvent::kMB);

    //Analysis Manager
    AliAnalysisManager* mgr = new AliAnalysisManager("testGridAnalysis");

    //ESD handler
    AliAODInputHandler*  aodH = new AliAODInputHandler( );
    mgr->SetInputEventHandler( aodH );

    //PID response
    TMacro addresp(gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C"));
    addresp.Exec(Form("%d", isMC));
    addresp.Exec(Form("%d", recoPass));

    //Multiplicity selection
    TMacro multadd=(gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C"));
    AliMultSelectionTask* multSelectionTask = reinterpret_cast<AliMultSelectionTask*>(multadd.Exec(Form("%d", 0)));
 
    // Connect plug-in to the analysis manager
    mgr->AddTask(task);

    // Create containers for input/output
    AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer( );
    AliAnalysisDataContainer *coutputA = mgr->CreateContainer( "AnalysisResultData", TList::Class( ), AliAnalysisManager::kOutputContainer, "AnalysisResultData.root" );
    
    // Connect input/output
    mgr->ConnectInput( task, 0, cinput );
    mgr->ConnectOutput( task, 1, coutputA );

     // add your task to the manager
     if(!mgr->InitAnalysis()) return;
     mgr->SetDebugLevel(2);
     mgr->PrintStatus();
     mgr->SetUseProgressBar(1, 25);

     if (local) {
        TChain* chain = new TChain("aodTree");
        chain->Add("alien:///alice/data/2010/LHC10h/000139510/ESDs/pass2/AOD160/0160/AliAOD.root");
        mgr->StartAnalysis("local", chain);
     } else {
//      AliAnalysisGrid *alienHandler = reinterpret_cast<AliAnalysisGrid*>(gInterpreter->LoadMacro("AlienHandler.C()"));
//      if (!alienHandler) return;
        AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
        alienHandler->SetOverwriteMode();
        alienHandler->SetRunMode(Form("%s", runMode.Data())); //full//terminate
        alienHandler->SetCheckCopy(kFALSE);
        
        alienHandler->SetAPIVersion("V1.1x");
        alienHandler->SetAliPhysicsVersion("vAN-20220923_ROOT6-1");
        alienHandler->SetGridDataDir("/alice/data/2010/LHC10h");
        alienHandler->SetDataPattern("/ESDs/pass2/AOD160/*AOD.root");
        alienHandler->SetRunPrefix("000");
        alienHandler->AddIncludePath("$ALICE_PHYSICS/include/");
        alienHandler->AddIncludePath("$ALICE_PHYSICS/OADB/macros/");
    
        std::vector<Int_t> runNumbers = {139510, 139507, 139505, 139503, 139465, 139438, 139437, 139360, 139329, 139328};
        
        for (auto run : runNumbers) {
          alienHandler->AddRunNumber(run);
        }
    
        alienHandler->SetGridWorkingDir("PbPb_Data");
        alienHandler->SetGridOutputDir( "output");
    
        alienHandler->SetAnalysisSource("AliAnalysisTaskMyCorrection.cxx");
        alienHandler->SetAdditionalLibs("AliAnalysisTaskMyCorrection.h AliAnalysisTaskMyCorrection.cxx");
    
        alienHandler->SetDefaultOutputs(kTRUE);
        alienHandler->SetAnalysisMacro("Task.C");
        alienHandler->SetSplitMaxInputFileNumber(100);
        alienHandler->SetExecutable("Task.sh");
        alienHandler->SetTTL(30000);
    
        alienHandler->SetInputFormat("xml-single");
        alienHandler->SetJDLName("Task.jdl");
        alienHandler->SetPrice(1);
        alienHandler->SetSplitMode("se");
        alienHandler->SetDefaultOutputs(kFALSE);
        alienHandler->SetOutputFiles("AnalysisResultData.root");
        alienHandler->SetClearPackages ( kFALSE );
  
        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetKeepLogs(kTRUE);
        alienHandler->SetMaxMergeStages(3);
        alienHandler->SetMergeViaJDL(kFALSE);
  
        // Enable debug printouts
        // mgr->SetDebugLevel(0);
  
        mgr->SetGridHandler(alienHandler);
        mgr->StartAnalysis("grid");
        gObjectTable->Print();
   }
}

void SetupEnvironment()
{
    // ROOT
    gSystem->Load ( "libCore.so" );
    gSystem->Load ( "libGeom.so" );
    gSystem->Load ( "libVMC.so" );
    gSystem->Load ( "libPhysics.so" );
    gSystem->Load ( "libTree.so" );
    gSystem->Load ( "libMinuit.so" );
    // AliROOT
    gSystem->Load ( "libSTEERBase.so" );
    gSystem->Load ( "libESD.so" );
    gSystem->Load ( "libAOD.so" );
    gSystem->Load ( "libANALYSIS.so" );
    gSystem->Load ( "libANALYSISalice.so" );
    gSystem->Load ( "libOADB.so" );

    // for running with root only
    gSystem->Load( "libTree.so" );
    gSystem->Load( "libGeom.so" );
    gSystem->Load( "libVMC.so" );
    gSystem->Load( "libPhysics.so" );

    //load analysis framework
    gSystem->Load( "libANALYSIS" );
    gSystem->Load( "libANALYSISalice" );

    //add include path
    gSystem->AddIncludePath( "-I$ALICE_ROOT/include");
    gSystem->AddIncludePath( "-I$ALICE_PHYSICS/include" );
    gSystem->AddIncludePath( "-I$ALICE_PHYSICS/OADB/macros" );
    gSystem->AddIncludePath( "-I$ALICE_ROOT/PHOS" );
   #if !defined (__CINT__) || defined (__CLING__)
       gInterpreter->ProcessLine(".include $ROOTSYS/include");
       gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
       gInterpreter->ProcessLine(".include $ALICE_PHYSICS/include");
   #else
       gROOT->ProcessLine(".include $ROOTSYS/include");
       gROOT->ProcessLine(".include $ALICE_ROOT/include");
       gROOT->ProcessLine(".include $ALICE_PHYSICS/include");
   #endif
}

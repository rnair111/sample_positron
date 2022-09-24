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
    gROOT->LoadMacro( "AliAnalysisTaskMyCorrection.cxx+g" );

    // Add collision centrality task
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C");

}






void run( Bool_t isMC=kFALSE)

{
    SetupEnvironment();

    // Use AliRoot includes to compile our task
    gROOT->ProcessLine(".include $ALICE_ROOT/include");;


    // Create and configure the alien handler plugin

    gROOT->LoadMacro("AlienHandler.C");

    AliAnalysisGrid *alienHandler = CreateAlienHandler();
    if (!alienHandler) return;

    //Analysis Manager
    AliAnalysisManager* mgr = new AliAnalysisManager("testGridAnalysis");

    //Create Task
    AliAnalysisTaskMyCorrection* task = new AliAnalysisTaskMyCorrection();
    if(!task) return;
    task->SelectCollisionCandidates(AliVEvent::kMB);


    //ESD handler
    AliAODInputHandler*  aodH = new AliAODInputHandler( );
    mgr->SetInputEventHandler( aodH );



    //PID Related Tasks
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
    AddTaskPIDResponse();


    // Connect plug-in to the analysis manager
    mgr->SetGridHandler(alienHandler);
    mgr->AddTask(task);




    // Create containers for input/output
    AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer( );
    AliAnalysisDataContainer *coutputA = mgr->CreateContainer( "AnalysisResultData", TList::Class( ), AliAnalysisManager::kOutputContainer, "AnalysisResultData.root" );
    
    // Connect input/output
    mgr->ConnectInput( task, 0, cinput );
    mgr->ConnectOutput( task, 1, coutputA );

    // Enable debug printouts
    // mgr->SetDebugLevel(0);

    if ( !mgr->InitAnalysis( ) ) return;
    mgr->PrintStatus( );

    // local, proof, grid
    mgr->StartAnalysis("grid");
    //mgr->StartAnalysis("local",chain);
    gObjectTable->Print();

}


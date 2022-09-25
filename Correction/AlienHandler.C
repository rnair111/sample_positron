
AliAnalysisGrid* CreateAlienHandler()
{

    AliAnalysisAlien *plugin = new AliAnalysisAlien();
    plugin->SetOverwriteMode();
    plugin->SetRunMode("terminate");//(can be "full", "test", or "terminate")
    plugin->SetCheckCopy(kFALSE);

    plugin->SetAPIVersion("V1.1x");
    plugin->SetAliPhysicsVersion("vAN-20191125-1");

    plugin->SetGridDataDir("/alice/sim/LHC11a10a_bis/");
    plugin->SetDataPattern("/AOD162/*/AliAOD.root");
    plugin->AddIncludePath("$ALICE_PHYSICS/include/");
    plugin->AddIncludePath("$ALICE_PHYSICS/OADB/macros/");



    plugin->AddRunNumber(139510);
    plugin->AddRunNumber(139507);
    plugin->AddRunNumber(139505);
    plugin->AddRunNumber(139503);
    plugin->AddRunNumber(139465);
    plugin->AddRunNumber(139438);
    plugin->AddRunNumber(139437);
    plugin->AddRunNumber(139360);
    plugin->AddRunNumber(139329);
    plugin->AddRunNumber(139328);
    plugin->AddRunNumber(139314);
    plugin->AddRunNumber(139310);
    plugin->AddRunNumber(139309);
    plugin->AddRunNumber(139173);


    plugin->SetGridWorkingDir("PbPb_MC1M");
    plugin->SetGridOutputDir( "PbPb_MC1M");

    plugin->SetAnalysisSource("AliAnalysisTaskMyCorrection.cxx");
    plugin->SetAdditionalLibs("AliAnalysisTaskMyCorrection.h AliAnalysisTaskMyCorrection.cxx");

    plugin->SetDefaultOutputs();
    plugin->SetAnalysisMacro("Task.C");
    plugin->SetSplitMaxInputFileNumber(100);
    plugin->SetExecutable("Task.sh");
    plugin->SetTTL(30000);

    plugin->SetInputFormat("xml-single");
    plugin->SetJDLName("Task.jdl");
    plugin->SetPrice(1);

plugin->SetMergeViaJDL(kFALSE);
plugin->SetMaxMergeStages(1);
    plugin->SetSplitMode("se");
    plugin->SetDefaultOutputs(kFALSE);
    plugin->SetOutputFiles("AnalysisResultMC.root");
    plugin->SetAliRootMode ( "default" );
    plugin->SetClearPackages ( kFALSE );
    plugin->SetFileForTestMode ( "files.txt" ); // file should contain path name to a local directory
    return plugin;
}

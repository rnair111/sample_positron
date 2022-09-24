AliAnalysisGrid* CreateAlienHandler()
{

    AliAnalysisAlien *plugin = new AliAnalysisAlien();
    plugin->SetOverwriteMode();
    plugin->SetRunMode("full");////full//terminate
    plugin->SetCheckCopy(kFALSE);
    
    plugin->SetAPIVersion("V1.1x");
    plugin->SetAliPhysicsVersion("vAN-20191028-1");
    plugin->SetGridDataDir("/alice/data/2010/LHC10h");
    plugin->SetDataPattern("/ESDs/pass2/AOD160/*AOD.root");
    plugin->SetRunPrefix("000");
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
    plugin->AddRunNumber(139328);/*

    plugin->AddRunNumber(139314);
    plugin->AddRunNumber(139310);
    plugin->AddRunNumber(139309);
    plugin->AddRunNumber(139173);
    plugin->AddRunNumber(139107);
    plugin->AddRunNumber(139105);
    plugin->AddRunNumber(139038);
    plugin->AddRunNumber(139037);
    plugin->AddRunNumber(139036);
    plugin->AddRunNumber(139029);
    plugin->AddRunNumber(139028);*/

    plugin->SetGridWorkingDir("PbPb_Data");
    plugin->SetGridOutputDir( "PbPb_Data");

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
    plugin->SetSplitMode("se");
    plugin->SetDefaultOutputs(kFALSE);
    plugin->SetOutputFiles("AnalysisResultData.root");
    plugin->SetClearPackages ( kFALSE );
    return plugin;
}

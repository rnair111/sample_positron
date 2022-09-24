#ifndef ALIANALYSISTASKMYCORRECTION_H
#define ALIANALYSISTASKMYCORRECTION_H

#include "AliPIDResponse.h"
#include "AliAnalysisTaskSE.h"

class TList;
class TH2F;
class TH1F;
class TH2D;
class TH3F;
class TString;
class AliMCEvent;
class AliStack;
class TParticle;
class AliCentrality;
class TObjArray;
class AliInputEventHandler;
class AliAODEvent;
class AliAODTrack;
class AliAODMCParticle;
class AliAODHandler;
class AliAODInputHandler;
class AliPIDResponse;

class AliAnalysisTaskMyCorrection : public AliAnalysisTaskSE {
public:
    AliAnalysisTaskMyCorrection();
    virtual ~AliAnalysisTaskMyCorrection ();


    virtual void   UserCreateOutputObjects();
    virtual void   UserExec(Option_t *option);
    virtual void   Terminate(Option_t *);

    Bool_t IsAODEventAccepted(AliAODEvent *event );//Event Accepted Or Not

    Double_t GetNsigmaTPC(AliPIDResponse* fPIDResponse , AliAODTrack* track , Int_t specie);
    Double_t GetEnergyOfTrackRec(AliAODTrack* track, Double_t mass);
    Int_t indexofSmallestElement(Double_t* array, Int_t size);




private:
    AliAODEvent*   fAOD;
    TClonesArray*  fArrayMC;
    TList*         fList;

    Double_t fNSigmaPID;


    TH2F *fHistZetaPtPlotData;
    TH1F* fHistEventCounter;
    TH2F* fHistdEdxTPCBeforeCut;
    TH2F* fHistdEdxTPCAfterCut; 
    TH1F* fHistPtPositron;
    TH1F* fHistPhiPositron;
    TH1F* fHistPtElectron;
    TH1F* fHistPhiElectron;
    TH1F* fHistEtaPositron;
    TH1F* fHistEtaElectron;
    TH1F* fHistEnergyPositron;
    TH1F* fHistEnergyElectron; 
 

    AliPIDResponse *fPIDResponse;//! PID response object


    Double_t fVzMax;//vzmax


    Int_t fAODTrackCutBit;//track cut bit from track selection (only used for AODs)

    Double_t fMinPtElectron, fMaxPtElectron;
    Double_t fMinEta, fMaxEta;//


    ClassDef(AliAnalysisTaskMyCorrection, 0);
};
#endif
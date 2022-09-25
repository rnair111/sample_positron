#ifndef ALIANALYSISTASKMYCorrection_H
#define ALIANALYSISTASKMYCorrection_H

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
class AliMultSelection;

class AliAnalysisTaskMyCorrection : public AliAnalysisTaskSE {
public:
    AliAnalysisTaskMyCorrection();
    virtual ~AliAnalysisTaskMyCorrection ();


    virtual void   UserCreateOutputObjects();
    virtual void   UserExec(Option_t *option);
    virtual void   Terminate(Option_t *);


    Bool_t IsAODEventAccepted(AliAODEvent *event );
    Double_t GetNsigmaTPC(AliPIDResponse* fPIDResponse , AliAODTrack* track , Int_t specie);



    Double_t GetEnergyOfTrackRec(AliAODTrack* track, Double_t mass);
    Double_t GetEnergyOfTrackGen(AliAODMCParticle * MCtrack, Double_t mass);
    Int_t GetCentralityClass(Double_t iCentPer);


private:
    AliAODEvent* fAOD;
    TClonesArray *fArrayMC; 
    

    TList        *fList;
    AliPIDResponse *fPIDResponse;//! PID response object
    Double_t fNSigmaPID;

    
    TH1F *fHistPhiMCPositronTruth[7];
    TH1F *fHistPtMCPositronTruth[7];
    TH1F *fHistEtaMCPositronTruth[7];
    TH1F *fHistZetaMCPositronTruth[7];
    TH1F *fHistPhiMCElectronTruth[7];
    TH1F *fHistPtMCElectronTruth[7];
    TH1F *fHistEtaMCElectronTruth[7];
    TH1F *fHistZetaMCElectronTruth[7]; 

    TH1F *fHistPtMCRecoPositron[7];
    TH1F *fHistPhiMCRecoPositron[7];
    TH1F *fHistPtMCRecoElectron[7];
    TH1F *fHistPhiMCRecoElectron[7];
    TH1F *fHistEtaMCRecoPositron[7];
    TH1F *fHistEtaMCRecoElectron[7];
    TH1F *fHistZetaMCRecoPositron[7];
    TH1F *fHistZetaMCRecoElectron[7]; 

    Double_t fVzMax;


    Int_t fAODTrackCutBit;//track cut bit from track selection (only used for AODs)

    Double_t fMinPtElectron, fMaxPtElectron;
    Double_t fMinEta, fMaxEta;

 

    ClassDef(AliAnalysisTaskMyCorrection, 0);
};
#endif
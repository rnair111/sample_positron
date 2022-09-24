#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TList.h"
#include "TObjArray.h"
#include "TString.h"
#include "TParticle.h"

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSE.h"
#include "AliCentrality.h"


#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAODHandler.h"
#include "AliAODInputHandler.h"
#include "AliAODpidUtil.h"
#include "AliAnalysisUtils.h"

#include "AliLog.h"
#include "AliInputEventHandler.h"
#include "AliAnalysisTaskMyCorrection.h"

ClassImp(AliAnalysisTaskMyCorrection)

//________________________________________________________________________
AliAnalysisTaskMyCorrection::AliAnalysisTaskMyCorrection():
    AliAnalysisTaskSE("taskPIDCorrection"),
    fAOD(0),
    fNSigmaPID(3.0),
    fArrayMC(0),
    fPIDResponse(0),
    fList(0),

    fVzMax(10.),
    fAODTrackCutBit(32),

    fMinPtElectron(0.20),
    fMaxPtElectron(10.0),
 

    fMinEta(-0.8),
    fMaxEta(0.8),
    
    fHistEventCounter(0),
    fHistdEdxTPCBeforeCut(0),
    fHistdEdxTPCAfterCut(0), 

    fHistPtPositron(0),
    fHistPhiPositron(0),
    fHistEtaPositron(0),
    fHistEnergyPositron(0),

    fHistPtElectron(0),
    fHistPhiElectron(0),
    fHistEtaElectron(0),
    fHistEnergyElectron(0)


{

    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());
}

AliAnalysisTaskMyCorrection::~AliAnalysisTaskMyCorrection()
{

}
void AliAnalysisTaskMyCorrection::UserCreateOutputObjects()
{
    fList = new TList();
    fList->SetName("QAListTruthReco");
    fList->SetOwner();

    fHistEventCounter = new TH1F("fHistEventCounter","Event Count (data): ",1,0.0,1.0);
    fHistEventCounter->Sumw2();
    fList->Add(fHistEventCounter);

    fHistdEdxTPCBeforeCut = new TH2F("fHistdEdxTPCBeforeCut", ";p_{T} (GeV/c);dE/dx (au.): Before Cut",1000,0.0,10.0, 1000, 0., 1000.);
    fHistdEdxTPCBeforeCut->Sumw2();
    fList->Add(fHistdEdxTPCBeforeCut);


    fHistdEdxTPCAfterCut = new TH2F("fHistdEdxTPCAfterCut", ";p_{T} (GeV/c);dE/dx (au.): After Cut",1000,0.0,10.0, 1000, 0., 1000.);
    fHistdEdxTPCAfterCut->Sumw2();
    fList->Add(fHistdEdxTPCAfterCut);

 


//Positron Distributions
    fHistPtPositron = new TH1F("fHistPtPositron","#frac{dN}{dp_{T}} : Reconstructed : #pi^{+}: ",225,0.2,0.65);
    fHistPtPositron->Sumw2();
    fList->Add(fHistPtPositron);

    fHistPhiPositron = new TH1F("fHistPhiPositron","#frac{dN}{d#phi} : Reconstructed : #pi^{+}: ",200,0.0,2*TMath::Pi());
    fHistPhiPositron->Sumw2();
    fList->Add(fHistPhiPositron);

    fHistEtaPositron = new TH1F("fHistEtaPositron","#frac{dN}{d#eta} : Reconstructed : #pi^{+}: ",200,-1.0,1.0);
    fHistEtaPositron->Sumw2();
    fList->Add(fHistEtaPositron);

    fHistEnergyPositron = new TH1F("fHistEnergyPositron","#frac{dN}{d#zeta} : Reconstructed : #pi^{+}: ",200,7.5,9.5);
    fHistEnergyPositron->Sumw2();
    fList->Add(fHistEnergyPositron);


//Electron Distributions
    fHistPtElectron = new TH1F("fHistPtElectron","#frac{dN}{dp_{T}}  : Reconstructed : #pi^{-}:",225,0.2,0.65);
    fHistPtElectron->Sumw2();
    fList->Add(fHistPtElectron);
    
    fHistPhiElectron = new TH1F("fHistPhiElectron","#frac{dN}{d#phi}  : Reconstructed : #pi^{-}:",200,0.0,2*TMath::Pi());
    fHistPhiElectron->Sumw2();
    fList->Add(fHistPhiElectron);
    
    fHistEtaElectron = new TH1F("fHistEtaElectron","#frac{dN}{d#eta}  : Reconstructed : #pi^{-}:",200,-1.0,1.0);
    fHistEtaElectron->Sumw2();
    fList->Add(fHistEtaElectron);

    fHistEnergyElectron = new TH1F("fHistEnergyElectron","#frac{dN}{d#zeta}  : Reconstructed : #pi^{-}:",200,7.5,9.5);
    fHistEnergyElectron->Sumw2();
    fList->Add(fHistEnergyElectron);

    PostData(1, fList);

}

Int_t AliAnalysisTaskMyCorrection::indexofSmallestElement(Double_t* array, Int_t size)
{
Int_t index = 0;

if (size != 1)
{

    Int_t n = array[0];
    for (Int_t i = 1; i < size; i++)
    {
        if (array[i] < n)
        {
            n = array[i];
            index = i;
        }
    }
}
return index;
}

Double_t AliAnalysisTaskMyCorrection::GetEnergyOfTrackRec(AliAODTrack* track,Double_t mass)
{

    Double_t Pz  = track->Pz();
    Double_t P1  = track->P();
    Double_t Pt  = track->Pt();

    Double_t E = TMath::Sqrt( (Pz * Pz) + (mass * mass) + (Pt * Pt));
    return E;
}


Double_t AliAnalysisTaskMyCorrection::GetNsigmaTPC(AliPIDResponse* fPIDresponse , AliAODTrack* track , Int_t specie)
{

    Double_t nSigmaElectron = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kElectron));
    Double_t nSigmaMuon     = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kMuon));
    Double_t nSigmaPion     = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kPion));
    Double_t nSigmaKaon     = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kKaon));
    Double_t nSigmaProton   = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kProton));
    
    if(specie == 0)      return nSigmaElectron;
    else if(specie == 1) return nSigmaMuon;
    else if(specie == 2) return nSigmaPion;
    else if(specie == 3) return nSigmaKaon;
    else if(specie == 4) return nSigmaProton;
}

Bool_t AliAnalysisTaskMyCorrection::IsAODEventAccepted(AliAODEvent *event)
{

    //Trigger Selection
    UInt_t maskPhysSel = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected();
    if((maskPhysSel & AliVEvent::kMB)==0) return kFALSE;

    const AliAODVertex *vertex = event->GetPrimaryVertex();
    if(vertex)
    {
        if(TMath::Abs(vertex->GetZ()) < fVzMax)
        {
            return kTRUE;
        }
        else return kFALSE;
    }
    else return kFALSE;

}

void AliAnalysisTaskMyCorrection::UserExec(Option_t *)
{

    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
    if (!fAOD) {
        printf("ERROR: fAOD not available\n");
        return;
    }

    fPIDResponse = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->GetPIDResponse();
    if (!fPIDResponse) AliFatal("This Task needs the PID response attached to the inputHandler");


    if(!IsAODEventAccepted(fAOD)) return; // Event Selection


    Double_t CentPercent = -1;
    AliAODHeader *header = (AliAODHeader*) fAOD->GetHeader();
    if(header) CentPercent = header->GetCentralityP()->GetCentralityPercentile("V0M");
    if(CentPercent < 0.0 || CentPercent > 80.0) return;

    Double_t MassElectron     = 0.000511; // GeV/c2 

    Short_t   jChargeReco;
    Double_t  jEtaReco;
    Double_t  jThetaReco;
    Double_t  jKaonYReco;
    Double_t  jProtonYReco;
    Double_t  jPhiReco;
    Double_t  jPtReco;
    Double_t  jPReco;
    Double_t  jYReco;
    Double_t  jCosReco;
    
    fHistEventCounter->Fill(0.5);

    for(Int_t jTracks = 0; jTracks < fAOD->GetNumberOfTracks(); jTracks++)
    {
        AliAODTrack *aodTrack = dynamic_cast<AliAODTrack*>(fAOD->GetTrack(jTracks));
        if(!aodTrack) continue;
        if (!aodTrack->TestFilterBit(fAODTrackCutBit)) continue;

        fHistdEdxTPCBeforeCut->Fill(aodTrack->P(),aodTrack->GetTPCsignal());


        jChargeReco = aodTrack->Charge();
        jEtaReco    = aodTrack->Eta();
        jCosReco    = TMath::Cos(aodTrack->Theta());
        jThetaReco  = aodTrack->Theta()*TMath::RadToDeg();
        jPhiReco    = aodTrack->Phi();
        jPtReco     = aodTrack->Pt();
        jPReco      = aodTrack->P();
        Int_t SignR = (jEtaReco >= 0 )? -1:1;

        //Kinematic Cuts
        if(jChargeReco == 0) continue;
        if(TMath::Abs(jEtaReco) > fMaxEta) continue;

        // Nsigma For Various Species
        Double_t dEdx=-1.0;
        Double_t nsigmaTPC[5] = {999.};

        //TPC and TOF Related Checking FLAGS
        Bool_t IsTPCSignal=kFALSE;

        //Check TPC and TOF PID Status
        dEdx = TMath::Abs(aodTrack->GetTPCsignal());
        if(dEdx > 0.0) IsTPCSignal=kTRUE;
        if(IsTPCSignal== kFALSE) continue;
        

              //Assign Species
        Int_t MostProbableSpecies =  -1;

        for(int iSpecies=0; iSpecies < 5; iSpecies++) //AliPID:enum EParticleType  = kElectron    kMuon   kPion   kKaon   kProton
        {

            nsigmaTPC[iSpecies]  = TMath::Abs(GetNsigmaTPC(fPIDResponse, aodTrack, iSpecies));
        }



                    if( (nsigmaTPC[0] <= fNSigmaPID ) )  MostProbableSpecies = 0;


        if(MostProbableSpecies == 0)
        {


            if((jPtReco <= fMaxPtPion ) && (jPtReco >= fMinPtPion))
            {
            	
            	fHistdEdxTPCAfterCut->Fill(aodTrack->P(),aodTrack->GetTPCsignal());
                
                Double_t   EnergyValue     = TMath::Abs(GetEnergyOfTrackRec(aodTrack,MassPion));



                if(jChargeReco > 0) {
                    fHistPhiPositron->Fill(jPhiReco);
                    fHistPtPositron->Fill(jPtReco);
                    fHistEtaPositron->Fill(jEtaReco);
                    fHistEnergyPositron->Fill(EnergyValue);
 

                }//Charge

                else if(jChargeReco < 0) {
                    fHistPhiElectron->Fill(jPhiReco);
                    fHistPtElectron->Fill(jPtReco);
                    fHistEtaElectron->Fill(jEtaReco);
                    fHistEnergyElectron->Fill(EnergyValue);
 
                    


                }//Charge
            }//Condition
        }//Species

    }//AOD Track Loop Reco

}//FunctionEnd

void AliAnalysisTaskMyCorrection::Terminate(Option_t *)
{

//Termination: Nothing Here as it causes crash while mearging

}

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
#include "AliMultSelection.h"

#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAODHandler.h"
#include "AliAODInputHandler.h"
#include "AliAODpidUtil.h"
#include "AliAnalysisUtils.h"
#include "AliAODMCParticle.h"

#include "AliLog.h"
#include "AliInputEventHandler.h"
#include "AliAnalysisTaskMyCorrection.h"

ClassImp(AliAnalysisTaskMyCorrection)
//________________________________________________________________________
AliAnalysisTaskMyCorrection::AliAnalysisTaskMyCorrection()
: AliAnalysisTaskSE("taskPIDCorrection"),
    fAOD(0),
    fNSigmaPID(3.0),
    fArrayMC(0),
    fPIDResponse(0),
    fList(0),
    fVzMax(10.),
    fAODTrackCutBit(32),
    fMinPtElectron(0.20),
    fMaxPtElectron(0.60),
    fMinEta(-0.8),
    fMaxEta(0.8)
 {   
    for (Int_t iCent = 0; iCent < 7; ++iCent) {
       fHistPtMCRecoPositron[iCent] = NULL;
       fHistPhiMCRecoPositron[iCent] = NULL;
       fHistPtMCRecoElectron[iCent] = NULL;
       fHistPhiMCRecoElectron[iCent] = NULL;
       fHistEtaMCRecoPositron[iCent] = NULL;
       fHistEtaMCRecoElectron[iCent] = NULL;

       fHistEnergyMCRecoPositron[iCent] = NULL;
       fHistEnergyMCRecoElectron[iCent] = NULL;

       fHistPtMCPositronTruth[iCent] = NULL;
       fHistPhiMCPositronTruth[iCent] = NULL;
       fHistPtMCElectronTruth[iCent] = NULL;
       fHistPhiMCElectronTruth[iCent] = NULL;
       fHistEtaMCPositronTruth[iCent] = NULL;
       fHistEtaMCElectronTruth[iCent] = NULL;

       fHistEnergyMCPositronTruth[iCent] = NULL;
       fHistEnergyMCElectronTruth[iCent] = NULL; 
    }
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());
}

AliAnalysisTaskMyCorrection::~AliAnalysisTaskMyCorrection()
{

}

//-----------------------------------------------------Start OutPutObjects - Define Binning and Histogra,ms In This Function----------------------------------------------------------------------------------------------
void AliAnalysisTaskMyCorrection::UserCreateOutputObjects()
{
    fList = new TList();
    fList->SetName("List");
    fList->SetOwner();

        
        for (int iCent = 0; iCent < 7; ++iCent) {

        // Generated Level Kinematic Variables
        fHistPtMCPositronTruth[iCent]= new TH1F(Form("fHistPtMCPositronTruth[%d]",iCent),"#frac{dN}{dp_{T}} : Generated : #pi^{+}: ",225,0.2,0.65);
        fHistPtMCPositronTruth[iCent]->Sumw2();
        fList ->Add(fHistPtMCPositronTruth[iCent]);

        fHistPtMCElectronTruth[iCent] =new TH1F(Form("fHistPtMCElectronTruth[%d]",iCent),"#frac{dN}{dp_{T}}   Generated : #pi^{-}:",225,0.2,0.65);
        fHistPtMCElectronTruth[iCent]->Sumw2();
        fList ->Add(fHistPtMCElectronTruth[iCent]);

        fHistPhiMCPositronTruth[iCent] = new TH1F(Form("fHistPhiMCPositronTruth[%d]",iCent),"#frac{dN}{d#phi} : Generated : #pi^{+}: ",200,0.0,2*TMath::Pi());
        fHistPhiMCPositronTruth[iCent]->Sumw2();
        fList ->Add(fHistPhiMCPositronTruth[iCent]);

        fHistPhiMCElectronTruth[iCent] = new TH1F(Form("fHistPhiMCElectronTruth[%d]",iCent),"#frac{dN}{d#phi} :Generated : #pi^{-}:",200,0.0,2*TMath::Pi());
        fHistPhiMCElectronTruth[iCent]->Sumw2();
        fList ->Add(fHistPhiMCElectronTruth[iCent]);

        fHistEtaMCPositronTruth[iCent] = new TH1F(Form("fHistEtaMCPositronTruth[%d]",iCent),"#frac{dN}{d#eta} : Generated : #pi^{+}: ",200,-1.0,1.0);
        fHistEtaMCPositronTruth[iCent]->Sumw2();
        fList->Add(fHistEtaMCPositronTruth[iCent]);

        fHistEtaMCElectronTruth[iCent] = new TH1F(Form("fHistEtaMCElectronTruth[%d]",iCent),"#frac{dN}{d#eta}   Generated : #pi^{-}:",200,-1.0,1.0);
        fHistEtaMCElectronTruth[iCent]->Sumw2();
        fList->Add(fHistEtaMCElectronTruth[iCent]);

        fHistEnergyMCPositronTruth[iCent] = new TH1F(Form("fHistEnergyMCPositronTruth[%d]",iCent),"#frac{dN}{d#zeta} : Generated : #pi^{+}: ",200,0.0,2.0);
        fHistEnergyMCPositronTruth[iCent]->Sumw2();
        fList->Add(fHistEnergyMCPositronTruth[iCent]);

        fHistEnergyMCElectronTruth[iCent] = new TH1F(Form("fHistEnergyMCElectronTruth[%d]",iCent),"#frac{dN}{d#zeta}   Generated : #pi^{-}:",200,0.0,2.0);
        fHistEnergyMCElectronTruth[iCent]->Sumw2();
        fList->Add(fHistEnergyMCElectronTruth[iCent]);


// Reconstructed Level Kinematic Variables

        fHistPtMCRecoPositron[iCent] = new TH1F(Form("fHistPtMCRecoPositron[%d]",iCent),"#frac{dN}{dp_{T}} : Reconstructed : #pi^{+}: ",225,0.2,0.65);
        fHistPtMCRecoPositron[iCent]->Sumw2();
        fList->Add(fHistPtMCRecoPositron[iCent]);

        fHistPtMCRecoElectron[iCent] = new TH1F(Form("fHistPtMCRecoElectron[%d]",iCent),"#frac{dN}{dp_{T}}  : Reconstructed : #pi^{-}:",225,0.2,0.65);
        fHistPtMCRecoElectron[iCent]->Sumw2();
        fList->Add(fHistPtMCRecoElectron[iCent]);


        fHistPhiMCRecoPositron[iCent] = new TH1F(Form("fHistPhiMCRecoPositron[%d]",iCent),"#frac{dN}{d#phi} : Reconstructed : #pi^{+}: ",200,0.0,2*TMath::Pi());
        fHistPhiMCRecoPositron[iCent]->Sumw2();
        fList->Add(fHistPhiMCRecoPositron[iCent]);

        fHistPhiMCRecoElectron[iCent] = new TH1F(Form("fHistPhiMCRecoElectron[%d]",iCent),"#frac{dN}{d#phi}  : Reconstructed : #pi^{-}:",200,0.0,2*TMath::Pi());
        fHistPhiMCRecoElectron[iCent]->Sumw2();
        fList->Add(fHistPhiMCRecoElectron[iCent]);

        fHistEtaMCRecoPositron[iCent] = new TH1F(Form("fHistEtaMCRecoPositron[%d]",iCent),"#frac{dN}{d#eta} : Reconstructed : #pi^{+}: ",200,-1.0,1.0);
        fHistEtaMCRecoPositron[iCent]->Sumw2();
        fList->Add(fHistEtaMCRecoPositron[iCent]);

        fHistEtaMCRecoElectron[iCent] = new TH1F(Form("fHistEtaMCRecoElectron[%d]",iCent),"#frac{dN}{d#eta}  : Reconstructed : #pi^{-}:",200,-1.0,1.0);
        fHistEtaMCRecoElectron[iCent]->Sumw2();
        fList->Add(fHistEtaMCRecoElectron[iCent]);

        fHistEnergyMCRecoPositron[iCent] = new TH1F(Form("fHistEnergyMCRecoPositron[%d]",iCent),"#frac{dN}{d#zeta} : Reconstructed : #pi^{+}: ",200,0.0,2.0);
        fHistEnergyMCRecoPositron[iCent]->Sumw2();
        fList->Add(fHistEnergyMCRecoPositron[iCent]);

        fHistEnergyMCRecoElectron[iCent] = new TH1F(Form("fHistEnergyMCRecoElectron[%d]",iCent),"#frac{dN}{d#zeta}  : Reconstructed : #pi^{-}:",200,0.0,2.0);
        fHistEnergyMCRecoElectron[iCent]->Sumw2();
        fList->Add(fHistEnergyMCRecoElectron[iCent]); 


    }//Centrality Loop


        PostData(1, fList);

}
//-----------------------------------------UserExec------------------------------------------------------------------------//
void AliAnalysisTaskMyCorrection::UserExec(Option_t *)
{


    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
    if (!fAOD) {
        printf("ERROR: fAOD not available\n");
        return;
    }
 
    fArrayMC = dynamic_cast<TClonesArray*>(fAOD->FindListObject(AliAODMCParticle::StdBranchName()));
    if (!fArrayMC)
        AliFatal("No array of MC particles found !!!");

   fPIDResponse = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->GetPIDResponse();
   if (!fPIDResponse) AliFatal("This Task needs the PID response attached to the inputHandler");

   if(!IsAODEventAccepted(fAOD)) return; // Event Selection

//    Double_t CentPercent = -1;
//    AliAODHeader *header = (AliAODHeader*) fAOD->GetHeader();
//    if(header) CentPercent = header->GetCentralityP()->GetCentralityPercentile("V0M");
//    if(CentPercent < 0.0 || CentPercent > 80.0) return;
//    Int_t cent = GetCentralityClass(CentPercent);
//    if(cent == -1) return;
   
    Double_t CentPercent = -1;
    //AliAODHeader *header = (AliAODHeader*) fAOD->GetHeader();
    AliMultSelection *multSelection =static_cast<AliMultSelection*>(fAOD->FindListObject("MultSelection"));
    if(multSelection) CentPercent = multSelection->GetMultiplicityPercentile("V0M");
    //if(header) CentPercent = header->GetCentralityP()->GetCentralityPercentile("V0M");
    if(CentPercent < 0.0 || CentPercent > 80.0) return;
    Int_t cent = GetCentralityClass(CentPercent);
    if(cent == -1) return;

    Double_t MassElectron     = 0.000511; // GeV/c2 

//-------------------------------------------------MCAOD Generated track looping----------------------------------------------------------------------------------------------------------
    Short_t   iCharge;
    Double_t  iEta;
    Double_t  iTheta;
    Double_t  iPionY;
    Double_t  iKaonY;
    Double_t  iProtonY;
    Double_t  iPhi;
    Double_t  iPt;
    Double_t  iP;
    Double_t  iYCut;
    Double_t  iCos; 
    Double_t  iEnergy;
    Double_t  iMass;

  //  Int_t nMCParticles = fArrayMC->GetEntriesFast();

    for(Int_t iParticle = 0; iParticle < fArrayMC->GetEntriesFast(); iParticle++)
    {


        AliAODMCParticle* genTrack = (AliAODMCParticle*) fArrayMC->At(iParticle);
        if (!genTrack)
        {
            AliError(Form("ERROR: Could not receive track %d (mc loop)", iParticle));
            continue;
        }

        if(!genTrack->IsPhysicalPrimary()) continue; //https://twiki.cern.ch/twiki/bin/viewauth/ALICE/TrackParametersMCTruth#Selecting_primary_particles
      //  if(genTrack->IsSecondaryFromWeakDecay()) continue;

        Int_t pdgCode= genTrack->GetPdgCode();


        iCharge = genTrack->Charge();
        iEta    = genTrack->Eta();
        iCos    = fabs(TMath::Cos(genTrack->Theta()));
        iTheta  = genTrack->Theta()* TMath::RadToDeg();
        iPhi    = genTrack->Phi();
        iPt     = genTrack->Pt();
        iP      = genTrack->P();
        iYCut   = genTrack->Y();
        iMass   = genTrack->M();
        Int_t SignG = (iEta >= 0.0 )? -1:1;
        iEnergy     =  TMath::Abs(GetEnergyOfTrackGen(genTrack,iMass)); 
        
          if(iCharge == 0) continue;
          if(fabs(iEta) > fMaxEta) continue;
          


//Fill the Plots gor Generated Pions
        if(TMath::Abs(pdgCode) == 11)
        {
            // Applying  Generated Level Kinematic Cuts
            if ((iPt >= fMinPtElectron) && (iPt <= fMaxPtElectron))
            {

               if(iCharge > 0) {
                fHistPhiMCPositronTruth[6]->Fill(iPhi);
                fHistPtMCPositronTruth[6]->Fill(iPt);
                fHistEtaMCPositronTruth[6]->Fill(iEta);
                fHistEnergyMCPositronTruth[6]->Fill(iEnergy); 

                fHistPhiMCPositronTruth[cent]->Fill(iPhi);
                fHistPtMCPositronTruth[cent]->Fill(iPt);
                fHistEtaMCPositronTruth[cent]->Fill(iEta);
                fHistEnergyMCPositronTruth[cent]->Fill(iEnergy); 
                }

                else if(iCharge < 0) {
                fHistPhiMCElectronTruth[6]->Fill(iPhi);
                fHistPtMCElectronTruth[6]->Fill(iPt);
                fHistEtaMCElectronTruth[6]->Fill(iEta);
                fHistEnergyMCElectronTruth[6]->Fill(iEnergy); 

                fHistPhiMCElectronTruth[cent]->Fill(iPhi);
                fHistPtMCElectronTruth[cent]->Fill(iPt);
                fHistEtaMCElectronTruth[cent]->Fill(iEta);
                fHistEnergyMCElectronTruth[cent]->Fill(iEnergy); 
            }//Charge
        }

        } //pion
       

    }//Generated Track Loop


//------------------------------------------------- Reconstructed TRACK LOOPING----------------------------------------------------------------------------------------------------------//

    Short_t   jChargeReco;
    Double_t  jEtaReco;
    Double_t  jThetaReco;
    Double_t  jPionYReco;
    Double_t  jKaonYReco;
    Double_t  jProtonYReco;
    Double_t  jPhiReco;
    Double_t  jPtReco;
    Double_t  jPReco;
    Double_t  jYReco;
    Double_t  jCosReco;
    Double_t  jEnergyReco;

    for(Int_t jTracks = 0; jTracks < fAOD->GetNumberOfTracks(); jTracks++)
    {
        AliAODTrack *aodTrack = dynamic_cast<AliAODTrack*>(fAOD->GetTrack(jTracks));
        if(!aodTrack) continue;

        if (!aodTrack->TestFilterBit(fAODTrackCutBit)) continue; 

        Int_t label = TMath::Abs(aodTrack->GetLabel());
        jChargeReco = aodTrack->Charge();
        jEtaReco    = aodTrack->Eta();
        jCosReco    = fabs(TMath::Cos(aodTrack->Theta()));
        jThetaReco  = aodTrack->Theta()*TMath::RadToDeg();
        jPhiReco    = aodTrack->Phi();
        jPtReco     = aodTrack->Pt();
        jPReco      = aodTrack->P();
        Int_t SignR = (jEtaReco >= 0 )? -1:1;
        if(jChargeReco == 0) continue;

        //Kinematic Cuts
        if(jChargeReco == 0) continue;
        if(fabs(jEtaReco) > fMaxEta) continue;


//--------------------------------------------------Get Gen track from reco by label------------------------------------------------------------------------------------------------

        AliAODMCParticle* recoMC = static_cast<AliAODMCParticle*>(fArrayMC->At(TMath::Abs(aodTrack->GetLabel())));
        if(!recoMC) continue;
        if (!recoMC->IsPhysicalPrimary()) continue;
        //if(recoMC->IsSecondaryFromWeakDecay()) continue;
        Int_t pdgCodeReco = ((AliAODMCParticle*)recoMC)->GetPdgCode();
        //if(fabs(pdgCodeReco) == 11) continue;

//---------------------------------------------------------- PID Scheme Implementation -----------------------------------------------------------------------------------//

        // Nsigma For Various Species
        Double_t dEdx=-1.0;

        Double_t nsigmaTPC[5] = {999.0};

  //TPC and TOF Related Checking FLAGS
        Bool_t IsTPCSignal=kFALSE;

        //Check TPC and TOF PID Status
        dEdx = fabs(aodTrack->GetTPCsignal());
        if(dEdx > 0.0) IsTPCSignal=kTRUE;
        if(IsTPCSignal== kFALSE) continue;
        

              //Assign Species
        Int_t MostProbableSpecies =  -1;


        for(int iSpecies=0; iSpecies < 5; iSpecies++) //AliPID:enum EParticleType  = kElectron    kMuon   kPion   kKaon   kProton
        {

            nsigmaTPC[iSpecies]  = TMath::Abs(GetNsigmaTPC(fPIDResponse, aodTrack, iSpecies));
        }


                    if( (nsigmaTPC[0] <= fNSigmaPID ) )  MostProbableSpecies = 0; 
     
//-------------------------------------------------------------------------Pion FILLING ---------------------------------------------------------------------------------------//

        if(TMath::Abs(pdgCodeReco) == 11 && MostProbableSpecies == 0)
        {

            if((jPtReco <= fMaxPtElectron ) && (jPtReco >= fMinPtElectron))
            {
                Double_t    jEnergyReco     = TMath::Abs(GetEnergyOfTrackRec(aodTrack,MassElectron)); 

                    if(jChargeReco > 0) {
                    fHistPhiMCRecoPositron[6]->Fill(jPhiReco);
                    fHistPtMCRecoPositron[6]->Fill(jPtReco);
                    fHistEtaMCRecoPositron[6]->Fill(jEtaReco);
                    fHistEnergyMCRecoPositron[6]->Fill(jEnergyReco); 

                    fHistPhiMCRecoPositron[cent]->Fill(jPhiReco);
                    fHistPtMCRecoPositron[cent]->Fill(jPtReco);
                    fHistEtaMCRecoPositron[cent]->Fill(jEtaReco);
                    fHistEnergyMCRecoPositron[cent]->Fill(jEnergyReco); 
                    }

                    else if(jChargeReco < 0) {
                    fHistPhiMCRecoElectron[6]->Fill(jPhiReco);
                    fHistPtMCRecoElectron[6]->Fill(jPtReco);
                    fHistEtaMCRecoElectron[6]->Fill(jEtaReco);
                    fHistEnergyMCRecoElectron[6]->Fill(jEnergyReco);
                    //fHistCosMCRecoElectron[6]->Fill(jCosReco);
                    //fHistPtSqMCRecoElectron[6]->Fill(jPtReco*jPtReco);

                    fHistPhiMCRecoElectron[cent]->Fill(jPhiReco);
                    fHistPtMCRecoElectron[cent]->Fill(jPtReco);
                    fHistEtaMCRecoElectron[cent]->Fill(jEtaReco);
                    fHistEnergyMCRecoElectron[cent]->Fill(jEnergyReco);
                    //fHistCosMCRecoElectron[cent]->Fill(jCosReco);
                    //fHistPtSqMCRecoElectron[cent]->Fill(jPtReco*jPtReco);

                }//Charge
            }//Condition
        }//Species

    }//AOD Track Loop Reco

}//FunctionEnd

void AliAnalysisTaskMyCorrection::Terminate(Option_t *)
{

//Termination: Nothing Here as it causes crash while mearging

}

Int_t AliAnalysisTaskMyCorrection::GetCentralityClass(Double_t iCentPer)
{
    if(iCentPer >=0.0 && iCentPer <= 5.0)             return 0;
    else if(iCentPer >=5.00 && iCentPer <= 10.0)      return 1;
    else if(iCentPer >=10.0 && iCentPer <= 20.0)      return 2;
    else if(iCentPer >=20.0 && iCentPer <= 40.0)      return 3;
    else if(iCentPer >=40.0 && iCentPer <= 60.0)      return 4;
    else if(iCentPer >=60.0 && iCentPer <= 80.0)      return 5;
    else return -1;
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

Double_t AliAnalysisTaskMyCorrection::GetEnergyOfTrackRec(AliAODTrack* track,Double_t mass)
{

    Double_t Pz  = track->Pz();
    Double_t P1  = track->P();
    Double_t Pt  = track->Pt();

    return TMath::Sqrt( (Pz * Pz) + (mass * mass) + (Pt * Pt)); 

}
//-----------------------------------------------------------------------------
Double_t AliAnalysisTaskMyCorrection::GetEnergyOfTrackGen(AliAODMCParticle * MCtrack, Double_t  mass)
{
    Double_t Pz  = MCtrack->Pz();
    Double_t P1  = MCtrack->P();
    Double_t Pt  = MCtrack->Pt();
    return TMath::Sqrt( (Pt * Pt) + (mass * mass) + (Pz * Pz) );
}

Double_t AliAnalysisTaskMyCorrection::GetNsigmaTPC(AliPIDResponse* fPIDresponse , AliAODTrack* track , Int_t specie)
{

    Double_t nSigmaElectron = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kElectron));
    Double_t nSigmaMuon     = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kMuon));
    Double_t nSigmaPion     = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kPion));
    Double_t nSigmaKaon     = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kKaon));
    Double_t nSigmaProton   = TMath::Abs(fPIDResponse->NumberOfSigmasTPC(track,(AliPID::EParticleType)AliPID::kProton));
    
    if(specie == 0) return nSigmaElectron;
    else if(specie == 1) return nSigmaMuon;
    else if(specie == 2) return nSigmaPion;
    else if(specie == 3) return nSigmaKaon;
    else if(specie == 4) return nSigmaProton;
    return 0;
}

//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B2EventAction.cc
/// \brief Implementation of the B2EventAction class

#include "B2EventAction.hh"
#include "B2TrackerHit.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "Analysis.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2EventAction::B2EventAction()
: G4UserEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2EventAction::~B2EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2EventAction::BeginOfEventAction(const G4Event*)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2EventAction::EndOfEventAction(const G4Event* event)
{
  // get number of stored trajectories

  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  // periodic printing

  G4int eventID = event->GetEventID();
  /*
  if ( eventID < 100 || eventID % 100 == 0) {
    G4cout << ">>> Event: " << eventID  << G4endl;
    if ( trajectoryContainer ) {
      G4cout << "    " << n_trajectories
             << " trajectories stored in this event." << G4endl;
    }
  }
  */

  G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
  /*
    G4cout << "    "  
           << hc->GetSize() << " hits stored in this event" << G4endl;
  */

  //Collect energy deposited inside the scintillators
  G4double Edep_Scint[4];
  G4double Time_Scint[4];
  G4double X_Scint[4];
  G4double Y_Scint[4];

  for(int iS=0;iS<4;++iS){
    Edep_Scint[iS]=0.;
    Time_Scint[iS]=0.;
  }

  for(int iH=0;iH<hc->GetSize();++iH){
    B2TrackerHit* hit=static_cast<B2TrackerHit*>(hc->GetHit(iH));
    Edep_Scint[hit->GetChamberNb()]+=hit->GetEdep(); //adding the energies of the steps inside each detector, identified with chamber number
    Time_Scint[hit->GetChamberNb()]+=hit->GetTime();
    X_Scint[hit->GetChamberNb()]+=hit->GetPos().getX();
    Y_Scint[hit->GetChamberNb()]+=hit->GetPos().getY();
  }
  for(int iS=0;iS<4;++iS){
    Time_Scint[iS]/=hc->GetSize();
    X_Scint[iS]/=hc->GetSize();
    Y_Scint[iS]/=hc->GetSize();
  }


//Filling the ntuple
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  for(int iS=0;iS<4;++iS){
    analysisManager->FillNtupleDColumn(iS,Edep_Scint[iS]);
    analysisManager->FillNtupleDColumn(iS+4,Time_Scint[iS]);
    analysisManager->FillNtupleDColumn(iS+8,X_Scint[iS]);
    analysisManager->FillNtupleDColumn(iS+12,Y_Scint[iS]);

  }
  analysisManager->AddNtupleRow(); 

}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

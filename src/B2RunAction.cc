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
/// \file B2RunAction.cc
/// \brief Implementation of the B2RunAction class

#include "B2RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

#include "Analysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2RunAction::B2RunAction()
 : G4UserRunAction()
{ 
  // set printing event number per each 100 events
  //G4RunManager::GetRunManager()->SetPrintProgress(1000);     
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetDefaultFileType("root");
	analysisManager->SetFileName("test");
	analysisManager->SetFirstNtupleId(1);
	analysisManager->SetFirstHistoId(1);

	analysisManager->CreateNtuple("T","T");

	analysisManager->CreateNtupleDColumn("Edep_Scint0");
	analysisManager->CreateNtupleDColumn("Edep_Scint1");
	analysisManager->CreateNtupleDColumn("Edep_Scint2");
	analysisManager->CreateNtupleDColumn("Edep_Scint3");

	analysisManager->CreateNtupleDColumn("Time_Scint0");
	analysisManager->CreateNtupleDColumn("Time_Scint1");
	analysisManager->CreateNtupleDColumn("Time_Scint2");
	analysisManager->CreateNtupleDColumn("Time_Scint3");

	analysisManager->CreateNtupleDColumn("X_Scint0");
	analysisManager->CreateNtupleDColumn("X_Scint1");
	analysisManager->CreateNtupleDColumn("X_Scint2");
	analysisManager->CreateNtupleDColumn("X_Scint3");

	analysisManager->CreateNtupleDColumn("Y_Scint0");
	analysisManager->CreateNtupleDColumn("Y_Scint1");
	analysisManager->CreateNtupleDColumn("Y_Scint2");
	analysisManager->CreateNtupleDColumn("Y_Scint3");

	analysisManager->CreateNtupleDColumn("X_gen");
	analysisManager->CreateNtupleDColumn("Y_gen");

	analysisManager->CreateNtupleDColumn("costheta");
	analysisManager->CreateNtupleDColumn("phi");		

	analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2RunAction::~B2RunAction()
{

	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::BeginOfRunAction(const G4Run*)
{ 
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::EndOfRunAction(const G4Run* )
{

	G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
/// \file B2bDetectorConstruction.cc
/// \brief Implementation of the B2bDetectorConstruction class
 
#include "B2bDetectorConstruction.hh"
#include "B2bDetectorMessenger.hh"
#include "B2bChamberParameterisation.hh"
#include "B2TrackerSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B2bDetectorConstruction::fMagFieldMessenger = 0;
 
B2bDetectorConstruction::B2bDetectorConstruction()
:G4VUserDetectorConstruction(),
 fLogicTarget(NULL), fLogicChamber(NULL), 
 fTargetMaterial(NULL), fChamberMaterial(NULL), 
 fStepLimit(NULL), 
 fCheckOverlaps(true)
{
  fMessenger = new B2bDetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
B2bDetectorConstruction::~B2bDetectorConstruction()
{
  delete fStepLimit;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4VPhysicalVolume* B2bDetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::DefineMaterials()
{
  // Material definition 

  G4NistManager* nistManager = G4NistManager::Instance();

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR");
  
  // Lead defined using NIST Manager
  fTargetMaterial  = nistManager->FindOrBuildMaterial("G4_Pb");

  // Xenon gas defined using NIST Manager
  //E. Defining scintillator material (EJ-200)
  G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.0079 * g/mole);
  G4Element* elC = new G4Element("Carbon","C",6.,12.01*g/mole);

  fChamberMaterial = new G4Material("EJ200",1.023*g/cm3,2);    // To be defined
    fChamberMaterial->AddElement(elH,0.44);
    fChamberMaterial->AddElement(elC,0.56);

  //fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Xe");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B2bDetectorConstruction::DefineVolumes()
{
  G4Material* air  = G4Material::GetMaterial("G4_AIR");

  // Sizes of the principal geometrical components (solids)
  
  G4int NbOfChambers = 4;
  //G4double chamberSpacing = 5*cm; // from chamber center to center!

  //G4double chamberWidth = 30.0*cm; // width of the chambers
  //G4double targetLength =  5.0*cm; // full length of Target
  
  //G4double trackerLength = (NbOfChambers+1)*chamberSpacing;

  G4double worldLength = 100*cm;//1.2 * (2*targetLength + trackerLength);

  //G4double targetRadius  = 0.5*targetLength;   // Radius of Target
  //targetLength = 0.5*targetLength;             // Half length of the Target  
  //G4double trackerSize   = 0.5*trackerLength;  // Half length of the Tracker

  // Definitions of Solids, Logical Volumes, Physical Volumes

  // World

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  G4Box* worldS
    = new G4Box("world",                                    //its name
                worldLength/2,worldLength/2,worldLength/2); //its size
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,   //its solid
                 air,      //its material
                 "World"); //its name
  
  //  Must place the World Physical volume unrotated at (0,0,0).
  // 
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,               // no rotation
                 G4ThreeVector(), // at (0,0,0)
                 worldLV,         // its logical volume
                 "World",         // its name
                 0,               // its mother  volume
                 false,           // no boolean operations
                 0,               // copy number
                 fCheckOverlaps); // checking overlaps 

  // Target
  /*
  G4ThreeVector positionTarget = G4ThreeVector(0,0,-(targetLength+trackerSize));

  G4Tubs* targetS
    = new G4Tubs("target",0.,targetRadius,targetLength,0.*deg,360.*deg);
  fLogicTarget
    = new G4LogicalVolume(targetS, fTargetMaterial,"Target",0,0,0);
  
  new G4PVPlacement(0,              // no rotation
                    positionTarget, // at (x,y,z)
                    fLogicTarget,   // its logical volume
                    "Target",       // its name
                    worldLV,        // its mother volume
                    false,          // no boolean operations
                    0,              // copy number
                    fCheckOverlaps); // checking overlaps 

  G4cout << "Target is " << 2*targetLength/cm << " cm of "
         << fTargetMaterial->GetName() << G4endl;
  */
  
  // Tracker
 

  G4double scintX=30.*cm;
  G4double scintY=26.*cm;
  G4double scintZ=1.2*cm;

  G4double plane_position[4]={0.*cm,6.*cm,24.*cm,30.*cm};

  //definiton of telescope volume
  G4double telescope_length=plane_position[3]+4*cm;

  

  //telescope rotation
  G4RotationMatrix *rotation=new G4RotationMatrix; //define detector inclination
    rotation->rotateY(20*deg);

  G4Box* telescopeS
    = new G4Box("telescope",scintX*0.5,scintY*0.5,telescope_length*0.5);

  G4LogicalVolume* telescopeL
    =new G4LogicalVolume(telescopeS,air,"telescopeL",0,0,0);




  G4Box* chamberS
    = new G4Box("tracker",scintX*0.5,scintY*0.5,scintZ*0.5);
  fLogicChamber 
    = new G4LogicalVolume(chamberS,fChamberMaterial,"Chamber",0,0,0);

   
    //placement of telescope planes
  for(int iP=0;iP<4;++iP){
    new G4PVPlacement( 0,               // no rotation 
                   G4ThreeVector(0,0,-telescope_length*0.5+scintZ*0.5+plane_position[iP]+2*cm), // at (0,0,0) 24
                   fLogicChamber,         // its logical volume
                   "Scint0",         // its name
                   telescopeL,               // its mother  volume
                   false,           // no boolean operations
                   iP,               // copy number
                   fCheckOverlaps
      );
  }
  
  // placement of telescope
  new G4PVPlacement(rotation,
    G4ThreeVector(0,0,0),
    telescopeL,
    "telescope",
    worldLV,
    false,
    0,
    fCheckOverlaps
    );
  

  G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  fLogicChamber->SetVisAttributes(chamberVisAtt);
  
  // Example of User Limits
  //
  // Below is an example of how to set tracking constraints in a given
  // logical volume
  //
  // Sets a max step length in the tracker region, with G4StepLimiter
/*
  G4double maxStep = 0.5*chamberWidth;
  fStepLimit = new G4UserLimits(maxStep);
  trackerLV->SetUserLimits(fStepLimit);
  */
  
  /// Set additional contraints on the track, with G4UserSpecialCuts
  ///
  /// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
  /// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
  ///                                           maxLength,
  ///                                           maxTime,
  ///                                           minEkin));

  // Always return the physical world

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void B2bDetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  G4String ScintSDname = "ScintSD";
  B2TrackerSD* aScintSD = new B2TrackerSD(ScintSDname,
                                            "Scintillator");
  G4SDManager::GetSDMpointer()->AddNewDetector(aScintSD);
  SetSensitiveDetector( fLogicChamber,  aScintSD );

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  /*
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetTargetMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial =
              nistManager->FindOrBuildMaterial(materialName);

  if (fTargetMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fTargetMaterial = pttoMaterial;
        if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
        G4cout
          << G4endl 
          << "----> The target is made of " << materialName << G4endl;
     } else {
        G4cout
          << G4endl 
          << "-->  WARNING from SetTargetMaterial : "
          << materialName << " not found" << G4endl;
     }
  }
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetChamberMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial =
              nistManager->FindOrBuildMaterial(materialName);

  if (fChamberMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fChamberMaterial = pttoMaterial;
        if (fLogicChamber) fLogicChamber->SetMaterial(fChamberMaterial);
        G4cout
          << G4endl 
          << "----> The chambers are made of " << materialName << G4endl;
     } else {
        G4cout
          << G4endl
          << "-->  WARNING from SetChamberMaterial : "
          << materialName << " not found" << G4endl;
     }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void B2bDetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
  //nistManager->FindOrBuildMaterial("G4_AIR");


  
  // Lead defined using NIST Manager
  fTargetMaterial  = nistManager->FindOrBuildMaterial("G4_Pb");

  // Xenon gas defined using NIST Manager
  //E. Defining scintillator material (EJ-200)
  G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.0079 * g/mole);
  G4Element* elC = new G4Element("Carbon","C",6.,12.01*g/mole);
  /*
  fChamberMaterial = new G4Material("EJ200",1.023*g/cm3,2);    // To be defined
    fChamberMaterial->AddElement(elH,0.44);
    fChamberMaterial->AddElement(elC,0.56);
  */
  //fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Xe");

  G4double a, z, density;
  G4int nelements;
  // Air
  auto N = new G4Element("Nitrogen", "N", z = 7, a = 14.01 * g / mole);
  auto O = new G4Element("Oxygen", "O", z = 8, a = 16.00 * g / mole);
  fWorldMaterial = new G4Material("Air", density = 1.29 * mg / cm3, nelements = 2);
  fWorldMaterial->AddElement(N, 70. * perCent);
  fWorldMaterial->AddElement(O, 30. * perCent);
  //
  // Water
  auto H = new G4Element("Hydrogen", "H", z = 1, a = 1.01 * g / mole);
  /*
  auto water = new G4Material("Water", density = 1.0 * g / cm3, nelements = 2);
  water->AddElement(H, 2);
  water->AddElement(O, 1);
  */
  fChamberMaterial = new G4Material("Water", density = 1.0 * g / cm3, nelements = 2);
  fChamberMaterial->AddElement(H, 2);
  fChamberMaterial->AddElement(O, 1);

  // ------------ Generate & Add Material Properties Table ------------
  //
  std::vector<G4double> photonEnergy = {
    2.034 * eV, 2.068 * eV, 2.103 * eV, 2.139 * eV, 2.177 * eV, 2.216 * eV,
    2.256 * eV, 2.298 * eV, 2.341 * eV, 2.386 * eV, 2.433 * eV, 2.481 * eV,
    2.532 * eV, 2.585 * eV, 2.640 * eV, 2.697 * eV, 2.757 * eV, 2.820 * eV,
    2.885 * eV, 2.954 * eV, 3.026 * eV, 3.102 * eV, 3.181 * eV, 3.265 * eV,
    3.353 * eV, 3.446 * eV, 3.545 * eV, 3.649 * eV, 3.760 * eV, 3.877 * eV,
    4.002 * eV, 4.136 * eV
  };

  // Water
  std::vector<G4double> refractiveIndex1 = {
    1.3435, 1.344,  1.3445, 1.345,  1.3455, 1.346,  1.3465, 1.347,
    1.3475, 1.348,  1.3485, 1.3492, 1.35,   1.3505, 1.351,  1.3518,
    1.3522, 1.3530, 1.3535, 1.354,  1.3545, 1.355,  1.3555, 1.356,
    1.3568, 1.3572, 1.358,  1.3585, 1.359,  1.3595, 1.36,   1.3608
  };
  std::vector<G4double> absorption = {
    3.448 * m,  4.082 * m,  6.329 * m,  9.174 * m,  12.346 * m, 13.889 * m,
    15.152 * m, 17.241 * m, 18.868 * m, 20.000 * m, 26.316 * m, 35.714 * m,
    45.455 * m, 47.619 * m, 52.632 * m, 52.632 * m, 55.556 * m, 52.632 * m,
    52.632 * m, 47.619 * m, 45.455 * m, 41.667 * m, 37.037 * m, 33.333 * m,
    30.000 * m, 28.500 * m, 27.000 * m, 24.500 * m, 22.000 * m, 19.500 * m,
    17.500 * m, 14.500 * m
  };

  // Material properties can be added as arrays. However, in this case it is
  // up to the user to make sure both arrays have the same number of elements.
  G4double scintilFastArray[]{ 1.0, 1.0 };
  G4double energyArray[]{ 2.034 * eV, 4.136 * eV };
  G4int lenArray = 2;

  std::vector<G4double> scintilSlow = {
    0.01, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 8.00,
    7.00, 6.00, 4.00, 3.00, 2.00, 1.00, 0.01, 1.00, 2.00, 3.00, 4.00,
    5.00, 6.00, 7.00, 8.00, 9.00, 8.00, 7.00, 6.00, 5.00, 4.00
  };

  auto myMPT1 = new G4MaterialPropertiesTable();

  // Values can be added to the material property table individually.
  // With this method, spline interpolation cannot be set. Arguments
  // createNewKey and spline both take their default values of false.
  // Need to specify the number of entries (1) in the arrays, as an argument
  // to AddProperty.
  G4int numEntries = 1;
  myMPT1->AddProperty("RINDEX", &photonEnergy[0], &refractiveIndex1[0],
                      numEntries);

  for(size_t i = 1; i < photonEnergy.size(); ++i)
  {
    myMPT1->AddEntry("RINDEX", photonEnergy[i], refractiveIndex1[i]);
  }

  // Check that group velocity is calculated from RINDEX
  if(myMPT1->GetProperty("RINDEX")->GetVectorLength() !=
     myMPT1->GetProperty("GROUPVEL")->GetVectorLength())
  {
    G4ExceptionDescription ed;
    ed << "Error calculating group velocities. Incorrect number of entries "
          "in group velocity material property vector.";
    G4Exception("OpNovice::OpNoviceDetectorConstruction", "OpNovice001",
                FatalException, ed);
  }

  // Adding a property from two std::vectors. Argument createNewKey is false
  // and spline is true.
  myMPT1->AddProperty("ABSLENGTH", photonEnergy, absorption, false, true);

  // Adding a property using a C-style array.
  // Spline interpolation isn't used for scintillation.
  // Arguments spline and createNewKey both take default value false.
  myMPT1->AddProperty("SCINTILLATIONCOMPONENT1", energyArray, scintilFastArray,
                      lenArray);

  myMPT1->AddProperty("SCINTILLATIONCOMPONENT2", photonEnergy, scintilSlow,
                      false, true);
  myMPT1->AddConstProperty("SCINTILLATIONYIELD", 50. / MeV);
  myMPT1->AddConstProperty("RESOLUTIONSCALE", 1.0);
  myMPT1->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 1. * ns);
  myMPT1->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 10. * ns);
  myMPT1->AddConstProperty("SCINTILLATIONYIELD1", 0.8);
  myMPT1->AddConstProperty("SCINTILLATIONYIELD2", 0.2);
  std::vector<G4double> energy_water = {
    1.56962 * eV, 1.58974 * eV, 1.61039 * eV, 1.63157 * eV, 1.65333 * eV,
    1.67567 * eV, 1.69863 * eV, 1.72222 * eV, 1.74647 * eV, 1.77142 * eV,
    1.7971 * eV,  1.82352 * eV, 1.85074 * eV, 1.87878 * eV, 1.90769 * eV,
    1.93749 * eV, 1.96825 * eV, 1.99999 * eV, 2.03278 * eV, 2.06666 * eV,
    2.10169 * eV, 2.13793 * eV, 2.17543 * eV, 2.21428 * eV, 2.25454 * eV,
    2.29629 * eV, 2.33962 * eV, 2.38461 * eV, 2.43137 * eV, 2.47999 * eV,
    2.53061 * eV, 2.58333 * eV, 2.63829 * eV, 2.69565 * eV, 2.75555 * eV,
    2.81817 * eV, 2.88371 * eV, 2.95237 * eV, 3.02438 * eV, 3.09999 * eV,
    3.17948 * eV, 3.26315 * eV, 3.35134 * eV, 3.44444 * eV, 3.54285 * eV,
    3.64705 * eV, 3.75757 * eV, 3.87499 * eV, 3.99999 * eV, 4.13332 * eV,
    4.27585 * eV, 4.42856 * eV, 4.59258 * eV, 4.76922 * eV, 4.95999 * eV,
    5.16665 * eV, 5.39129 * eV, 5.63635 * eV, 5.90475 * eV, 6.19998 * eV
  };

  // Rayleigh scattering length is calculated by G4OpRayleigh

  // Mie: assume 100 times larger than the rayleigh scattering
  std::vector<G4double> mie_water = {
    167024.4 * m, 158726.7 * m, 150742 * m,   143062.5 * m, 135680.2 * m,
    128587.4 * m, 121776.3 * m, 115239.5 * m, 108969.5 * m, 102958.8 * m,
    97200.35 * m, 91686.86 * m, 86411.33 * m, 81366.79 * m, 76546.42 * m,
    71943.46 * m, 67551.29 * m, 63363.36 * m, 59373.25 * m, 55574.61 * m,
    51961.24 * m, 48527.00 * m, 45265.87 * m, 42171.94 * m, 39239.39 * m,
    36462.50 * m, 33835.68 * m, 31353.41 * m, 29010.30 * m, 26801.03 * m,
    24720.42 * m, 22763.36 * m, 20924.88 * m, 19200.07 * m, 17584.16 * m,
    16072.45 * m, 14660.38 * m, 13343.46 * m, 12117.33 * m, 10977.70 * m,
    9920.416 * m, 8941.407 * m, 8036.711 * m, 7202.470 * m, 6434.927 * m,
    5730.429 * m, 5085.425 * m, 4496.467 * m, 3960.210 * m, 3473.413 * m,
    3032.937 * m, 2635.746 * m, 2278.907 * m, 1959.588 * m, 1675.064 * m,
    1422.710 * m, 1200.004 * m, 1004.528 * m, 833.9666 * m, 686.1063 * m
  };

  // Mie: gforward, gbackward, forward backward ratio
  G4double mie_water_const[3] = { 0.99, 0.99, 0.8 };

  myMPT1->AddProperty("MIEHG", energy_water, mie_water, false, true);
  myMPT1->AddConstProperty("MIEHG_FORWARD", mie_water_const[0]);
  myMPT1->AddConstProperty("MIEHG_BACKWARD", mie_water_const[1]);
  myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO", mie_water_const[2]);

  G4cout << "Water G4MaterialPropertiesTable:" << G4endl;
  myMPT1->DumpTable();

  //water->SetMaterialPropertiesTable(myMPT1);
  fChamberMaterial->SetMaterialPropertiesTable(myMPT1);
  // Set the Birks Constant for the Water scintillator
  //water->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
  fChamberMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

  // Air
  std::vector<G4double> refractiveIndex2 = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0 };

  auto myMPT2 = new G4MaterialPropertiesTable();
  myMPT2->AddProperty("RINDEX", photonEnergy, refractiveIndex2);

  G4cout << "Air G4MaterialPropertiesTable:" << G4endl;
  myMPT2->DumpTable();

  fWorldMaterial->SetMaterialPropertiesTable(myMPT2);


  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B2bDetectorConstruction::DefineVolumes()
{
  //G4Material* air  = G4Material::GetMaterial("G4_AIR");

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
                 fWorldMaterial,      //its material
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
  //G4double telescope_length=plane_position[3]+4*cm;
  G4double scint_pair_length = 5*cm; 
  

  //telescope rotation
  G4RotationMatrix *rotation=new G4RotationMatrix; //define detector inclination
    rotation->rotateY(0*deg);

  G4Box* telescopeS
    = new G4Box("telescope",scintX*0.5,scintY*0.5,scint_pair_length*0.5);

  G4LogicalVolume* telescopeL
    =new G4LogicalVolume(telescopeS,fWorldMaterial,"telescopeL",0,0,0);




  G4Box* chamberS
    = new G4Box("tracker",scintX*0.5,scintY*0.5,scintZ*0.5);
  fLogicChamber 
    = new G4LogicalVolume(chamberS,fChamberMaterial,"Chamber",0,0,0);

  G4double positionChamber[2] = {-1.5*cm,1.5*cm};
    //placement of telescope planes
  for(int iP=0;iP<2;++iP){
    new G4PVPlacement( 0,               // no rotation 
                   G4ThreeVector(0,0,positionChamber[iP]), // at (0,0,0) 24
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
    "telescope_1_2",
    worldLV,
    false,
    0,
    fCheckOverlaps
    );

    new G4PVPlacement(rotation,
    G4ThreeVector(0,0,10*cm),
    telescopeL,
    "telescope_3_4",
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

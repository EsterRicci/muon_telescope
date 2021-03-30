# muon_telescope
# Written by E. Ricci (ester.ricci@unitn.it) for Experimental techniques in Nuclear and subnuclear physics course of Università di Trento.

Based on B2b example of GEANT4.

To compile the code:

$ mkdir build 
$ cd build
$ cmake <path to CMakeLists.txt file>
$ make

To run the code:

$ ./exampleB2b

Useful commands:

/run/beamOn <particle number>
  
/gun/particle <particle type (e-,e+,mu-,mu+,proton...)>

/gun/energy <energy value> <Units (MeV, GeV...)>

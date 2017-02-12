#include <iostream>
//#include "UTIL/LCTOOLS.h"
//#include "IOIMPL/LCCollectionIOVec.h"
//#include "UTIL/Operators.h"
//#include <UTIL/CellIDDecoder.h>
#include "DDRec/API/IDDecoder.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Objects.h"
#include "DDG4/Geant4Data.h"
// check directory
#include <sys/stat.h>
#include <TROOT.h>
#include <TFile.h>
#include <TVector3.h>
#include <TH1F.h>
#include <TH2F.h>
#include "TTree.h"
#include "TMath.h"
#include "time.h"
#include <dirent.h>
#include <string>
#include <vector>
#include <map>

void recon_test(
    const char* file_name          = "simple_example_out.root",
    const char* compact_file       = "compact/simple_example.xml"
    )
{

  using namespace DD4hep;

  DD4hep::Geometry::LCDD& lcdd = DD4hep::Geometry::LCDD::getInstance();
  lcdd.fromCompact(compact_file);

  // Magnetic Field
  const double position[3] = {0,0,0};
  double bField[3]         = {0,0,0}; 
  lcdd.field().magneticField(position,bField); 
  double _Bz = bField[2]/dd4hep::tesla; 
  std::cout << " Magnetic Field Bz = " << _Bz << std::endl;

  //DD4hep::DDRec::SurfaceManager& surfMan = *lcdd.extension<DD4hep::DDRec::SurfaceManager>() ;
  //_map = surfMan.map( det.name() ) ;
  //if( ! _map ) {   
  //  std::stringstream err  ; err << " Could not find surface map for detector: " 
  //                               << _subDetName << " in SurfaceManager " ;
  //  std::cout << err << std::endl;;
  //}

  DD4hep::Geometry::DetElement det = lcdd.detector( "TrackerPlanes" ) ;
  auto readout = lcdd.readout("TPCollection");
  auto seg     = readout.segmentation();
  auto id      = readout.idSpec();

  std::cout << " num col " << readout.numCollections() << std::endl;
  std::cout << " id spec " << id.toString() << std::endl;
  std::cout << " field "  << id.fieldDescription() << std::endl;
  std::cout << " decoder " << id.decoder() << std::endl;

  auto& id_decoder = DD4hep::DDRec::IDDecoder::getInstance();

  TFile * f = new TFile(file_name, "READ");
  if(!f) { std::cout << " root file not found\n";  exit -1; }
  TTree * t = (TTree*)f->FindObjectAny("EVENT");
  if(!t) { std::cout << " tree not found\n";  exit -1; }
  t->Print();

  std::vector<DD4hep::Simulation::Geant4Tracker::Hit*> * TP_hits = nullptr;
  t->SetBranchAddress("TPCollection",&TP_hits);

  int nentries = t->GetEntries();

  for(int i_event = 0; i_event < nentries; i_event++) {

    t->GetEntry(i_event);
    for(auto ahit : (*TP_hits)){

      std::cout << " Cell ID               " << ahit->cellID << std::endl;
      std::cout << " seg.type : " << seg.type() << std::endl;;

      std::cout << " Cell ID from position " << id_decoder.cellID(ahit->position/10.0) << std::endl;;

      std::cout << id_decoder.placement(ahit->cellID).toString() << std::endl;;
      std::cout << id_decoder.detectorElement(ahit->cellID).path() << std::endl;;
      std::cout << id_decoder.detectorElement(ahit->cellID).placementPath() << std::endl;;

      std::cout << " Volume ID " << id_decoder.volumeID(ahit->cellID) << std::endl;

      auto apos = id_decoder.position( ahit->cellID );

      std::cout << " cell pos : x=" << apos.x() << ", y=" << apos.y() << ", z=" << apos.z() << std::endl;

      //std::cout << ahit->length << std::endl;

    }  

  }
  return;

}

int main( int argc, char **argv )
{
  std::cout << "recon_test" << std::endl;
  recon_test();

   return 0;
}


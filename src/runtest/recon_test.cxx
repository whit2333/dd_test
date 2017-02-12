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
  //std::cout << "id encode " << id.encode({1,0,1,3,1,0,0}) << std::endl;
  std::cout << " id spec " << id.toString() << std::endl;
  std::cout << " field "  << id.fieldDescription() << std::endl;
  std::cout << " decoder " << id.decoder() << std::endl;
  auto bf = id.decoder();
  //std::cout << " barrel "   << bf->index("barrel")<<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].name() <<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].offset() <<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].width() <<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].value() <<std::endl;

  //auto files        = get_files_in_directory(DIRNAME);

  //IO::LCReader* lcReader = IOIMPL::LCFactory::getInstance()->createLCReader() ;
  //EVENT::LCEvent* evt = 0 ;
  //std::string mcpType("std::vector<IMPL::MCParticleImpl*>") ;
  //std::string mcpName("MCParticle") ;

  //int  nEvents      = 0  ;
  //std::vector<std::string> files = {"example1_ddsim_out.slcio"};
  //std::sort(files.begin(),files.end());

  auto& id_decoder = DD4hep::DDRec::IDDecoder::getInstance();

  TFile * f = new TFile(file_name, "READ");
  if(!f) { 
    std::cout << " root file not found\n";  
    exit -1;
  }
  f->ls();
  TTree * t = (TTree*)f->FindObjectAny("EVENT");
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


  //for(auto aFile : files) {

  //  std::cout << "Opening " << aFile << '\n';
  //  lcReader->open( aFile.c_str() ) ;
  //  n_files_read++;

  //  //----------- the event loop -----------
  //  while( (evt = lcReader->readNextEvent()) != 0 ) {

  //    if(nEvents%1000==0) {
  //      if(nEvents==0)
  //        UTIL::LCTOOLS::dumpEvent( evt ) ;
  //      std::cout << "Event " << nEvents << '\n';
  //    }

  //    IMPL::LCCollectionVec* thrown_col = (IMPL::LCCollectionVec*) evt->getCollection( "MCParticle"  ) ;
  //    IMPL::LCCollectionVec* simtrackhits_col = (IMPL::LCCollectionVec*) evt->getCollection("SiTrackerBarrelHits") ;

  //    UTIL::LCTOOLS::printSimTrackerHits( evt->getCollection("SiTrackerBarrelHits") ); 

  //    //UTIL::LCTOOLS::printMCParticles( col ) ;

  //    int nMCP    = thrown_col->getNumberOfElements() ;
  //    int nSimTrackHits = simtrackhits_col->getNumberOfElements() ;

  //    CellIDDecoder<SimTrackerHit> lcio_id(simtrackhits_col);

  //    for(int i_track=0 ; i_track<nSimTrackHits ; ++i_track) {
  //      auto  ahit = (EVENT::SimTrackerHit*) simtrackhits_col->getElementAt(i_track);
  //      //double hit_time  = ahit->getTime();
  //      //double EDep  = ahit->getEDep();
  //      //auto cell_id = ahit->getCellID0();
  //      std::cout << "id " << ahit->getCellID0() << std::endl;

  //      TVector3 x0(ahit->getPosition());
  //      TVector3 x1(ahit->getPosition());
  //      x1.Print();

  //      auto hit_pos = DD4hep::Geometry::Position(x0.x(),x0.y(),x0.z());
  //      std::cout << " hit  pos : x=" << hit_pos.x() << ", y=" << hit_pos.y() << ", z=" << hit_pos.z() << std::endl;

  //      auto volumeid = id_decoder.volumeID(hit_pos);
  //      std::cout << " volumeID : " << volumeid << std::endl;

  //      //auto cellid = lcio_id(const_cast<EVENT::SimTrackerHit*>(ahit)).getValue();//id_decoder.cellID(hit_pos);
  //      auto cellid = id_decoder.cellID(hit_pos);
  //      std::cout << " cellID : " << cellid << std::endl;

  //      auto bpos   = id_decoder.position(cellid);
  //      std::cout << " cell pos : x=" << bpos.x() << ", y=" << bpos.y() << ", z=" << bpos.z() << std::endl;

  //    }
  //    nEvents++;
  //  }
  //  // -------- end of event loop -----------

  //  lcReader->close() ;

  //  if(n_files_read >= max_files) break;
  //}

      //auto bf      = seg.decoder();
      //std::cout << bf->valueString() << std::endl;

      //auto pos     = seg.position(1000);
      //std::cout << " x=" << pos.x() << ", y=" << pos.y() << ", z=" << pos.z() << std::endl;

  //TCanvas * c = new TCanvas();
  //hR0->Draw();

  //c = new TCanvas();
  //hZR0->Draw("cols");
  return;

}

int main( int argc, char **argv )
{
  std::cout << "recon_test" << std::endl;
  recon_test();

   return 0;
}


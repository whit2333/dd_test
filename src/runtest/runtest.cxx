#include <iostream>
#include "UTIL/LCTOOLS.h"
#include "IOIMPL/LCCollectionIOVec.h"
#include "UTIL/Operators.h"
#include <UTIL/CellIDDecoder.h>
#include "DDRec/API/IDDecoder.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Objects.h"
// check directory
#include <sys/stat.h>
#include <TROOT.h>
#include <TFile.h>
#include <TVector3.h>
#include <TH1F.h>
#include <TH2F.h>
#include "TMath.h"
#include"time.h"
#include <dirent.h>
#include <string>
#include <vector>
#include <map>

void compact_geo_test2(
    const char* DIRNAME            = "data/tests",
    const char* result_file_prefix = "tests",
    //const char* compact_file       = "detectors/SiD/slic/sieic3/sieic3_compact.xml")
    const char* compact_file       = "compact/SiD.xml")
    //const char* compact_file       = "detectors/SiD/compact/sid_working/sidloi3_v00.xml")
{

  using namespace EVENT ;
  using namespace UTIL;
  using namespace IMPL ;

  DD4hep::Geometry::LCDD& lcdd = DD4hep::Geometry::LCDD::getInstance();
  lcdd.fromCompact(compact_file);
  const double position[3]={0,0,0}; // position to calculate magnetic field at (the origin in this case)
  double bField[3]={0,0,0}; 
  lcdd.field().magneticField(position,bField); 
  double _Bz = bField[2]/dd4hep::tesla; 

  std::cout << " Magnetic Field Bz = " << _Bz << std::endl;

  TH1F * hR0   = new TH1F("hR0","R;R [mm];",100,0,1000);
  TH1F * hZ0   = new TH1F("hZ0","Z;Z [mm];",100,-1000,1000);
  TH2F * hZR0   = new TH2F("hZR0","ZR;Z [mm];R [mm];",100,-1000,1000,100,0,1000);

  using namespace DD4hep;

  DD4hep::Geometry::DetElement det = lcdd.detector( "SiVertexBarrel" ) ;

  //DD4hep::DDRec::SurfaceManager& surfMan = *lcdd.extension<DD4hep::DDRec::SurfaceManager>() ;
  //_map = surfMan.map( det.name() ) ;

  //if( ! _map ) {   
  //  std::stringstream err  ; err << " Could not find surface map for detector: " 
  //                               << _subDetName << " in SurfaceManager " ;
  //  std::cout << err << std::endl;;
  //}

  auto readout = lcdd.readout("SiTrackerBarrelHits");
  auto det_el  = lcdd.detector("SiVertexBarrel");
  auto vol_man = lcdd.volumeManager();
  std::cout << " detector type   : " <<  det_el.type() << std::endl;
  std::cout << " detector key    : " <<  det_el.key() << std::endl;
  std::cout << " detector level  : " <<  det_el.level() << std::endl;
  std::cout << " detector id     : " <<  det_el.id() << std::endl;
  std::cout << " detector ppath  : " <<  det_el.placementPath() << std::endl;
  std::cout << " children        : " <<  det_el.children().size() << std::endl;
  auto children = det_el.children();
  for(auto& child  : children) {
    //std::cout << "   child        : " << child.first << std::endl;
    //std::cout << "   child type   : " <<  child.second.type() << std::endl;
    //std::cout << "   child key    : " <<  child.second.key() << std::endl;
    //std::cout << "   child level  : " <<  child.second.level() << std::endl;
    //std::cout << "   child id     : " <<  child.second.id() << std::endl;
    std::cout << "   grandchildren: " <<  child.second.children().size() << std::endl;
    for(auto& gc  : child.second.children()) {
      //std::cout << "     grandchild        : " <<  gc.first << std::endl;
      //std::cout << "     grandchild name   : " <<  gc.second.name() << std::endl;
      //std::cout << "     grandchild type   : " <<  gc.second.type() << std::endl;
      //std::cout << "     grandchild key    : " <<  gc.second.key() << std::endl;
      //std::cout << "     grandchild level  : " <<  gc.second.level() << std::endl;
      std::cout << "     grandgrandchildren: " <<  gc.second.children().size() << std::endl;
      for(auto& g2c  : gc.second.children()) {
        std::cout << "       g2child        : " <<  g2c.first << std::endl;
        std::cout << "       g2child name   : " <<  g2c.second.name() << std::endl;
        std::cout << "       g2child type   : " <<  g2c.second.type() << std::endl;
        std::cout << "       g2child key    : " <<  g2c.second.key() << std::endl;
        std::cout << "       g2child level  : " <<  g2c.second.level() << std::endl;
        std::cout << "       g3children     : " <<  g2c.second.children().size() << std::endl;
      }
    }
  }


  auto seg     = readout.segmentation();

  std::cout << " num col " << readout.numCollections() << std::endl;
  auto id      = readout.idSpec();
  //std::cout << "id encode " << id.encode({1,0,1,3,1,0,0}) << std::endl;
  std::cout << " id spec " << id.toString() << std::endl;
  std::cout << " field " << id.fieldDescription() << std::endl;
  std::cout << " decoder " << id.decoder() << std::endl;
  auto bf = id.decoder();
  //std::cout << " barrel "   << bf->index("barrel")<<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].name() <<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].offset() <<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].width() <<std::endl;
  //std::cout << " barrel "   << (*bf)["barrel"].value() <<std::endl;

  int  max_files    = 2;
  int  nEvents      = 0  ;
  int  n_files_read = 0;
  //auto files        = get_files_in_directory(DIRNAME);
  std::vector<std::string> files = {"example1_ddsim_out.slcio"};

  IO::LCReader* lcReader = IOIMPL::LCFactory::getInstance()->createLCReader() ;
  EVENT::LCEvent* evt = 0 ;
  std::string mcpType("std::vector<IMPL::MCParticleImpl*>") ;
  std::string mcpName("MCParticle") ;

  std::sort(files.begin(),files.end());

  auto& id_decoder = DD4hep::DDRec::IDDecoder::getInstance();

  for(auto aFile : files) {

    std::cout << "Opening " << aFile << '\n';
    lcReader->open( aFile.c_str() ) ;
    n_files_read++;

    //----------- the event loop -----------
    while( (evt = lcReader->readNextEvent()) != 0 ) {

      if(nEvents%1000==0) {
        if(nEvents==0)
          UTIL::LCTOOLS::dumpEvent( evt ) ;
        std::cout << "Event " << nEvents << '\n';
      }

      IMPL::LCCollectionVec* thrown_col = (IMPL::LCCollectionVec*) evt->getCollection( "MCParticle"  ) ;
      IMPL::LCCollectionVec* simtrackhits_col = (IMPL::LCCollectionVec*) evt->getCollection("SiTrackerBarrelHits") ;

      UTIL::LCTOOLS::printSimTrackerHits( evt->getCollection("SiTrackerBarrelHits") ); 

      //UTIL::LCTOOLS::printMCParticles( col ) ;

      int nMCP    = thrown_col->getNumberOfElements() ;
      int nSimTrackHits = simtrackhits_col->getNumberOfElements() ;

      CellIDDecoder<SimTrackerHit> lcio_id(simtrackhits_col);

      for(int i_track=0 ; i_track<nSimTrackHits ; ++i_track) {
        auto  ahit = (EVENT::SimTrackerHit*) simtrackhits_col->getElementAt(i_track);
        //double hit_time  = ahit->getTime();
        //double EDep  = ahit->getEDep();
        //auto cell_id = ahit->getCellID0();
        std::cout << "id " << ahit->getCellID0() << std::endl;

        TVector3 x0(ahit->getPosition());
        TVector3 x1(ahit->getPosition());
        x1.Print();

        auto hit_pos = DD4hep::Geometry::Position(x0.x(),x0.y(),x0.z());
        std::cout << " hit  pos : x=" << hit_pos.x() << ", y=" << hit_pos.y() << ", z=" << hit_pos.z() << std::endl;

        auto volumeid = id_decoder.volumeID(hit_pos);
        std::cout << " volumeID : " << volumeid << std::endl;

        //auto cellid = lcio_id(const_cast<EVENT::SimTrackerHit*>(ahit)).getValue();//id_decoder.cellID(hit_pos);
        auto cellid = id_decoder.cellID(hit_pos);
        std::cout << " cellID : " << cellid << std::endl;

        auto bpos   = id_decoder.position(cellid);
        std::cout << " cell pos : x=" << bpos.x() << ", y=" << bpos.y() << ", z=" << bpos.z() << std::endl;

      }
      nEvents++;
    }
    // -------- end of event loop -----------

    lcReader->close() ;

    if(n_files_read >= max_files) break;
  }

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
  std::cout << "test 1" << std::endl;
  compact_geo_test2();

   return 0;
}


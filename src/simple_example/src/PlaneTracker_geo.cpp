#include "DD4hep/DetFactoryHelper.h"

#include "DDRec/Surface.h"
#include "DDRec/DetectorData.h"
#include <exception>

using namespace DD4hep;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec ;
using namespace DDSurfaces ;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  
  xml_det_t    x_det = e;
  std::string  name  = x_det.nameStr();
  
  Assembly assembly( name+"_assembly" );
  
  // The top level detector element that this function returns
  DetElement tracker( name, x_det.id()  ) ;

  //DDRec::ZPlanarData*  zPlanarData = new DDRec::ZPlanarData ;
  // Loop over layer elements in xml

  for(xml_coll_t c(e, _U(layer) ); c; ++c)  {
    
    xml_comp_t alayer( c );
    
    // Parse the xml elements and get the required information
    
    // child elements: ladder and sensitive
    xml_comp_t z_sensitive( alayer.child( _U(sensitive) ));
    xml_comp_t z_module(  alayer.child( _U(module)  ));
    
    int    layer_id = alayer.id();
    double zoffset  = alayer.attr<double>(  _Unicode(z_offset) ) ;

    double supp_x_half    = z_module.attr<double>(  _Unicode(x_half) );
    double supp_y_half    = z_module.attr<double>(  _Unicode(y_half));
    double supp_thickness = z_module.attr<double>(  _Unicode(thickness));
    double sens_thickness = z_sensitive.attr<double>(  _Unicode(thickness));

    double supp_gap_half_width = 1.5; //cm

    std::string supp_matS = z_module.materialStr() ;
    std::string sens_matS = z_sensitive.materialStr() ;

    Material supp_mat     = lcdd.material( supp_matS ) ;
    Material sens_mat     = lcdd.material( sens_matS ) ;

    std::cout << supp_matS << std::endl;
    std::cout << sens_matS << std::endl;

    std::string layername = name + _toString(layer_id, "_layer%d");

    DetElement  layerDE( tracker , _toString(layer_id, "layer_%d"),  x_det.id() );
    DetElement  sensDE(  layerDE , _toString(layer_id, "module_%d"), x_det.id() );
   
    //--------------------------------

    Box supp_box( supp_x_half,     supp_y_half,     supp_thickness/2.0 );
    // There will be four sensitive elements for each layer
    Box sens_box( supp_x_half/2.0-supp_gap_half_width, supp_y_half/2.0-supp_gap_half_width, sens_thickness/2.0 );

    Volume support_vol( layername+"_supp", supp_box, supp_mat  );
    Volume sensor_vol( layername+"_sens", sens_box, sens_mat );

    std::string supp_vis  = z_module.visStr() ;
    std::string sens_vis  = z_sensitive.visStr() ;

    // -------- create a measurement plane for the tracking surface attched to the sensitive volume -----
    Vector3D u( 1. , 0. , 0. ) ;
    Vector3D v( 0. , 1. , 0. ) ;
    Vector3D n( 0. , 0. , 1. ) ;
    //Vector3D o( 0. , 0. , 0. ) ;

    // compute the inner and outer thicknesses that need to be assigned to the tracking surface
    // depending on wether the support is above or below the sensor
    // The tracking surface is used in reconstruction. It provides  material thickness
    // and radation lengths needed for various algorithms, routines, etc.
    double inner_thickness = supp_thickness/2.0;
    double outer_thickness = supp_thickness/2.0;
    
    SurfaceType type( SurfaceType::Sensitive ) ;
    VolPlane    surf( sensor_vol, type, inner_thickness , outer_thickness , u,v,n ) ; //,o ) ;

    //--------------------------------------------
     
    sens.setType("tracker");
    sensor_vol.setSensitiveDetector(sens);
    
    sensor_vol.setAttributes( lcdd, x_det.regionStr(), x_det.limitsStr(), sens_vis );
    support_vol.setAttributes( lcdd, x_det.regionStr(), x_det.limitsStr(), supp_vis );



    //  /* pv = */layer_assembly.placeVolume(support_vol,Transform3D(rot, Position((radius+lthick/2.)*cos(phi) - offset*sin(phi),
    //                                                                          (radius+lthick/2.)*sin(phi) + offset*cos(phi),
    //                                                                          0. ) ));

    // place the layer
    PlacedVolume support_pv = assembly.placeVolume( support_vol, Position(0,0, zoffset) );
    support_pv.addPhysVolID("layer", layer_id );  
    layerDE.setPlacement( support_pv ) ;

    for(int i_module=0; i_module<4; i_module++) {

      double xsign = (i_module%2==0 ? -1.0 : 1.0);
      double ysign = (i_module/2==0 ? -1.0 : 1.0);
      Position module_postion(xsign*supp_x_half/2.0, ysign*supp_y_half/2.0, 0.0);

      // Put the sensitve volume in the support volume
      PlacedVolume pv = support_vol.placeVolume(sensor_vol, module_postion);
      pv.addPhysVolID("module", i_module );  
      sensDE.setPlacement( pv );
    }
  }

  //tracker.addExtension< DDRec::ZPlanarData >( zPlanarData ) ;
  
  Volume       mother    = lcdd.pickMotherVolume( tracker ) ;
  PlacedVolume mother_pv = mother.placeVolume(assembly);
  mother_pv.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;
  tracker.setPlacement(mother_pv);
       
  assembly->GetShape()->ComputeBBox() ;

  return tracker;
}

DECLARE_DETELEMENT(PlaneTracker,create_element)

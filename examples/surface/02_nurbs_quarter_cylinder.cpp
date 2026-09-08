#include <cmath>
#include <iostream>
#include <opencagd/surface.h>
int main(){using opencagd::geometry::Point3D;const double w=std::sqrt(.5);opencagd::surface::NURBSSurface::ControlNet c={{Point3D(1,0,0),Point3D(1,0,1)},{Point3D(1,1,0),Point3D(1,1,1)},{Point3D(0,1,0),Point3D(0,1,1)}};opencagd::surface::NURBSSurface::WeightNet weights={{1,1},{w,w},{1,1}};opencagd::surface::NURBSSurface s(2,{0,0,0,1,1,1},1,{0,0,1,1},c,weights);auto p=s.evaluate(.5,.5);auto n=s.normal(.5,.5);std::cout<<"P="<<p.x()<<','<<p.y()<<','<<p.z()<<"\nN="<<n.x()<<','<<n.y()<<','<<n.z()<<'\n';return 0;}

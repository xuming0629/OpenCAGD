#include <iostream>
#include <opencagd/surface.h>
int main(){using opencagd::geometry::Point3D;opencagd::surface::BSplineSurface::ControlNet net={{Point3D(0,0,0),Point3D(0,1,0),Point3D(0,2,0)},{Point3D(1,0,0),Point3D(1,1,1),Point3D(1,2,0)},{Point3D(2,0,0),Point3D(2,1,0),Point3D(2,2,0)}};opencagd::surface::BSplineSurface s(2,{0,0,0,1,1,1},2,{0,0,0,1,1,1},net);auto p=s.evaluate(.5,.5);std::cout<<"center="<<p.x()<<','<<p.y()<<','<<p.z()<<"\nnormal="<<s.normal(.5,.5).x()<<','<<s.normal(.5,.5).y()<<','<<s.normal(.5,.5).z()<<'\n';return 0;}

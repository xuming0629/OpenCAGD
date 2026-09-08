#include <iostream>
#include <opencagd/surface.h>
int main(){using opencagd::geometry::Point3D;opencagd::surface::BezierSurface s({{Point3D(0,0,0),Point3D(0,1,0)},{Point3D(1,0,0),Point3D(1,1,1)}});auto p=s.evaluate(.5,.5);auto su=s.derivative(.5,.5,1,0);auto sv=s.derivative(.5,.5,0,1);auto n=s.normal(.5,.5);std::cout<<"P="<<p.x()<<','<<p.y()<<','<<p.z()<<"\nSu="<<su.x()<<','<<su.y()<<','<<su.z()<<"\nSv="<<sv.x()<<','<<sv.y()<<','<<sv.z()<<"\nN="<<n.x()<<','<<n.y()<<','<<n.z()<<'\n';return 0;}

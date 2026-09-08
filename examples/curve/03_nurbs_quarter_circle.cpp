#include <cmath>
#include <iostream>
#include <opencagd/curve.h>
int main(){ using opencagd::geometry::Point3D; const double w=std::sqrt(.5); opencagd::curve::NURBSCurve c(2,{0,0,0,1,1,1},{Point3D(1,0),Point3D(1,1),Point3D(0,1)},{1,w,1}); for(int i=0;i<=4;++i){double u=i/4.0;auto p=c.evaluate(u);std::cout<<"u="<<u<<" P="<<p.x()<<','<<p.y()<<" r="<<std::sqrt(p.x()*p.x()+p.y()*p.y())<<'\n';} std::cout<<"curvature(0.5)="<<c.curvature(.5)<<'\n'; return 0; }

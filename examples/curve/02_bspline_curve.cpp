#include <iostream>
#include <opencagd/curve.h>
int main(){ using opencagd::geometry::Point3D; opencagd::curve::BSplineCurve c(2,{0,0,0,1,2,3,3,3},{Point3D(0,0),Point3D(1,2),Point3D(2,1),Point3D(3,3),Point3D(4,0)}); const auto d=c.domain(); std::cout<<"domain=["<<d.min<<','<<d.max<<"]\n"; for(int i=0;i<=6;++i){double u=d.min+(d.max-d.min)*i/6.0;auto p=c.evaluate(u);std::cout<<"u="<<u<<" P="<<p.x()<<','<<p.y()<<'\n';} return 0; }

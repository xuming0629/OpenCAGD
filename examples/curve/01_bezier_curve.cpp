#include <iostream>
#include <opencagd/curve.h>
int main(){ using opencagd::geometry::Point3D; opencagd::curve::BezierCurve c({Point3D(0,0),Point3D(1,2),Point3D(3,2),Point3D(4,0)}); const double t=.4; auto p=c.evaluate(t);auto d=c.derivative(t);auto T=c.tangent(t);std::cout<<"degree="<<c.degree()<<"\nP="<<p.x()<<','<<p.y()<<"\nC'="<<d.x()<<','<<d.y()<<"\nT="<<T.x()<<','<<T.y()<<"\nkappa="<<c.curvature(t)<<'\n'; return 0; }

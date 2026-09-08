#include <iostream>
#include <opencagd/surface.h>
int main(){using opencagd::geometry::Point3D;opencagd::surface::BezierSurface s({{Point3D(0,0,0),Point3D(0,1,0),Point3D(0,2,0)},{Point3D(1,0,0),Point3D(1,1,1),Point3D(1,2,0)},{Point3D(2,0,0),Point3D(2,1,0),Point3D(2,2,0)}});auto dg=opencagd::surface::differential_geometry(s,.5,.5);std::cout<<"E,F,G = "<<dg.E<<", "<<dg.F<<", "<<dg.G<<"\n"<<"e,f,g = "<<dg.e<<", "<<dg.f<<", "<<dg.g<<"\n"<<"K="<<dg.gaussian_curvature<<" H="<<dg.mean_curvature<<"\n"<<"k1="<<dg.principal_curvature_max<<" k2="<<dg.principal_curvature_min<<'\n';return 0;}

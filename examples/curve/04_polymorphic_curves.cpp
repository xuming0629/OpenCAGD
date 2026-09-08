#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <opencagd/curve.h>
int main(){ using namespace opencagd; using geometry::Point3D; std::vector<std::unique_ptr<curve::ParametricCurve>> curves; curves.emplace_back(new curve::BezierCurve({Point3D(0,0),Point3D(1,2),Point3D(3,0)})); curves.emplace_back(new curve::BSplineCurve(2,{0,0,0,1,2,2,2},{Point3D(0,0),Point3D(1,2),Point3D(2,2),Point3D(3,0)})); const double w=std::sqrt(.5); curves.emplace_back(new curve::NURBSCurve(2,{0,0,0,1,1,1},{Point3D(1,0),Point3D(1,1),Point3D(0,1)},{1,w,1})); for(std::size_t i=0;i<curves.size();++i){auto d=curves[i]->domain();double u=(d.min+d.max)*.5;auto p=curves[i]->evaluate(u);auto t=curves[i]->tangent(u);std::cout<<i<<": P="<<p.x()<<','<<p.y()<<" T="<<t.x()<<','<<t.y()<<'\n';} return 0; }

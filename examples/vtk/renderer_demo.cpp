#include <opencagd/surface.h>
#include <opencagd/visualization.h>
#include <opencagd/vtk.h>
int main(){using opencagd::geometry::Point3D;opencagd::surface::BezierSurface::ControlNet net={{Point3D(0,0,0),Point3D(0,1,0),Point3D(0,2,0)},{Point3D(1,0,0),Point3D(1,1,1),Point3D(1,2,0)},{Point3D(2,0,0),Point3D(2,1,0),Point3D(2,2,0)}};opencagd::surface::BezierSurface s(net);auto grid=opencagd::surface::sample_surface(s,31,31);auto data=opencagd::visualization::make_surface_3d_data(grid,net);opencagd::visualization::append_surface_normals(data,s,5,5,.2);opencagd::visualization::RenderOptions o;o.title="OpenCAGD VTK backend";o.show=OPENCAGD_EXAMPLE_SHOW!=0;o.output_path=std::string(OPENCAGD_EXAMPLE_OUTPUT_DIR)+"/renderer_demo.png";opencagd::vtk::VTKRenderer().render(data,o);return 0;}

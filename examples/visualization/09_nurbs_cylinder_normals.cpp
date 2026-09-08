#include <cmath>
#include <vector>
#include <opencagd/surface.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    typedef opencagd::surface::NURBSSurface::ControlNet ControlNet;
    typedef opencagd::surface::NURBSSurface::WeightNet WeightNet;
    const double w = std::sqrt(2.0) / 2.0;
    const ControlNet control_net{
        {Point3D(1,0,0),Point3D(1,0,2)},
        {Point3D(1,1,0),Point3D(1,1,2)},
        {Point3D(0,1,0),Point3D(0,1,2)}};
    const WeightNet weights{{1,1},{w,w},{1,1}};
    const std::vector<double> knots_u{0,0,0,1,1,1};
    const std::vector<double> knots_v{0,0,1,1};
    const opencagd::surface::NURBSSurface cylinder(2,knots_u,1,knots_v,control_net,weights);
    const opencagd::surface::SurfaceSampleGrid grid = opencagd::surface::sample_surface(cylinder,65,45);
    opencagd::visualization::Surface3DData data =
        opencagd::visualization::make_surface_3d_data(grid, control_net);
    opencagd::visualization::append_surface_normals(data, cylinder, 6, 4, 0.20);

    opencagd::visualization::RenderOptions options;
    options.title = "Exact NURBS Quarter Cylinder with Surface Normals";
    options.xlabel = "x"; options.ylabel = "y"; options.zlabel = "z";
    opencagd_example::finalize_options(options, "09_nurbs_cylinder_normals.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

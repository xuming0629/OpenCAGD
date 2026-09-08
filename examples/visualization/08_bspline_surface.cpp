#include <vector>
#include <opencagd/surface.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    typedef opencagd::surface::BSplineSurface::ControlNet ControlNet;
    const std::vector<double> knots{0,0,0,0.33,0.66,1,1,1};
    const ControlNet control_net{
        {Point3D(-2,-2,0),Point3D(-2,-1,0.2),Point3D(-2,0,0),Point3D(-2,1,-0.2),Point3D(-2,2,0)},
        {Point3D(-1,-2,0),Point3D(-1,-1,1),Point3D(-1,0,1.4),Point3D(-1,1,0.5),Point3D(-1,2,0)},
        {Point3D(0,-2,0),Point3D(0,-1,1.5),Point3D(0,0,2.2),Point3D(0,1,1.5),Point3D(0,2,0)},
        {Point3D(1,-2,0),Point3D(1,-1,0.5),Point3D(1,0,1.4),Point3D(1,1,1),Point3D(1,2,0)},
        {Point3D(2,-2,0),Point3D(2,-1,-0.2),Point3D(2,0,0),Point3D(2,1,0.2),Point3D(2,2,0)}};
    const opencagd::surface::BSplineSurface surface(2,knots,2,knots,control_net);
    const opencagd::surface::SurfaceSampleGrid grid = opencagd::surface::sample_surface(surface,75,75);
    const opencagd::visualization::Surface3DData data =
        opencagd::visualization::make_surface_3d_data(grid, control_net);

    opencagd::visualization::RenderOptions options;
    options.title = "Quadratic B-Spline Surface";
    options.xlabel = "x"; options.ylabel = "y"; options.zlabel = "z";
    opencagd_example::finalize_options(options, "08_bspline_surface.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

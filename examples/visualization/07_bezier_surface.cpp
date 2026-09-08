#include <vector>
#include <opencagd/surface.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    typedef opencagd::surface::BezierSurface::ControlNet ControlNet;
    const ControlNet control_net{
        {Point3D(-1.5,-1.5,0.0),Point3D(-1.5,-0.5,0.3),Point3D(-1.5,0.5,0.3),Point3D(-1.5,1.5,0.0)},
        {Point3D(-0.5,-1.5,0.2),Point3D(-0.5,-0.5,1.6),Point3D(-0.5,0.5,1.4),Point3D(-0.5,1.5,0.2)},
        {Point3D(0.5,-1.5,0.2),Point3D(0.5,-0.5,1.4),Point3D(0.5,0.5,1.8),Point3D(0.5,1.5,0.2)},
        {Point3D(1.5,-1.5,0.0),Point3D(1.5,-0.5,0.3),Point3D(1.5,0.5,0.3),Point3D(1.5,1.5,0.0)}};
    const opencagd::surface::BezierSurface surface(control_net);
    const opencagd::surface::SurfaceSampleGrid grid = opencagd::surface::sample_surface(surface, 70, 70);
    const opencagd::visualization::Surface3DData data =
        opencagd::visualization::make_surface_3d_data(grid, control_net);

    opencagd::visualization::RenderOptions options;
    options.title = "Bicubic Bezier Surface and Control Net";
    options.xlabel = "x"; options.ylabel = "y"; options.zlabel = "z";
    opencagd_example::finalize_options(options, "07_bezier_surface.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

#include <vector>
#include <opencagd/curve.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    const std::vector<Point3D> control_points{
        Point3D(0.0,0.0), Point3D(1.0,3.0), Point3D(3.0,-1.5), Point3D(4.5,1.2)};
    const double t = 0.37;
    const opencagd::curve::BezierCurve curve(control_points);

    opencagd::visualization::DeCasteljauData data;
    data.levels = curve.construction_levels(t);
    data.parameter = t;

    opencagd::visualization::RenderOptions options;
    options.title = "De Casteljau Construction at t = 0.37";
    options.xlabel = "x"; options.ylabel = "y";
    opencagd_example::finalize_options(options, "02_bezier_de_casteljau.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

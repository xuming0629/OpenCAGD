#include <vector>
#include <opencagd/curve.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    const std::vector<Point3D> control_points{
        Point3D(0.0,0.0), Point3D(1.0,2.5), Point3D(2.4,-1.0), Point3D(4.0,1.5)};

    const opencagd::curve::BezierCurve curve(control_points);
    const std::vector<Point3D> samples = opencagd::curve::sample_curve(curve, 240);
    const opencagd::visualization::Curve2DData data =
        opencagd::visualization::make_curve_2d_data(samples, control_points, "Bezier curve");

    opencagd::visualization::RenderOptions options;
    options.title = "Cubic Bezier Curve and Control Polygon";
    options.xlabel = "x"; options.ylabel = "y";
    opencagd_example::finalize_options(options, "01_bezier_curve.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

#include <vector>
#include <opencagd/curve.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    const std::vector<Point3D> control_points{
        Point3D(0,0), Point3D(0.8,2.0), Point3D(1.8,2.8), Point3D(3,-1),
        Point3D(4,1.2), Point3D(5,0.2), Point3D(6,2)};
    const std::vector<double> knots{0,0,0,0,0.25,0.50,0.75,1,1,1,1};
    const opencagd::curve::BSplineCurve curve(3, knots, control_points);
    const std::vector<Point3D> samples = opencagd::curve::sample_curve(curve, 320);
    const opencagd::visualization::Curve2DData data =
        opencagd::visualization::make_curve_2d_data(samples, control_points, "B-Spline curve");

    opencagd::visualization::RenderOptions options;
    options.title = "Cubic B-Spline Curve";
    options.xlabel = "x"; options.ylabel = "y";
    opencagd_example::finalize_options(options, "05_bspline_curve.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

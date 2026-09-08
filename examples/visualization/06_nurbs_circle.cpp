#include <cmath>
#include <vector>
#include <opencagd/curve.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    const double w = std::sqrt(2.0) / 2.0;
    const std::vector<Point3D> control_points{
        Point3D(1,0),Point3D(1,1),Point3D(0,1),Point3D(-1,1),Point3D(-1,0),
        Point3D(-1,-1),Point3D(0,-1),Point3D(1,-1),Point3D(1,0)};
    const std::vector<double> weights{1,w,1,w,1,w,1,w,1};
    const std::vector<double> knots{0,0,0,0.25,0.25,0.50,0.50,0.75,0.75,1,1,1};
    const opencagd::curve::NURBSCurve circle(2, knots, control_points, weights);
    const std::vector<Point3D> samples = opencagd::curve::sample_curve(circle, 500);
    const opencagd::visualization::Curve2DData data =
        opencagd::visualization::make_curve_2d_data(samples, control_points, "exact NURBS circle");

    opencagd::visualization::RenderOptions options;
    options.title = "Exact Unit Circle with a Quadratic NURBS";
    options.xlabel = "x"; options.ylabel = "y";
    opencagd_example::finalize_options(options, "06_nurbs_circle.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

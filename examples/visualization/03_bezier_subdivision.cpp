#include <string>
#include <utility>
#include <vector>
#include <opencagd/curve.h>
#include "example_backend.h"

int main()
{
    using opencagd::geometry::Point3D;
    const std::vector<Point3D> control_points{
        Point3D(0.0,0.0), Point3D(1.0,2.5), Point3D(2.5,-1.2), Point3D(4.0,1.0)};
    const opencagd::curve::BezierCurve curve(control_points);
    const std::pair<opencagd::curve::BezierCurve, opencagd::curve::BezierCurve> parts = curve.subdivide(0.5);

    opencagd::visualization::Curve2DData data;
    opencagd::visualization::CurveSeries2D left, right;
    left.points = opencagd::curve::sample_curve(parts.first, 120);
    left.label = "left Bezier segment";
    right.points = opencagd::curve::sample_curve(parts.second, 120);
    right.label = "right Bezier segment";
    data.curves.push_back(left);
    data.curves.push_back(right);
    data.control_points = control_points;

    opencagd::visualization::RenderOptions options;
    options.title = "Bezier Subdivision at t = 0.5";
    options.xlabel = "x"; options.ylabel = "y";
    opencagd_example::finalize_options(options, "03_bezier_subdivision.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}

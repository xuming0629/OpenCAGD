#include <cmath>

#include <opencagd/curve/nurbs_curve.hpp>
#include <opencagd/curve/sampling.hpp>
#include <opencagd/visualization/matplotlib.hpp>

int main()
{
    const double w = std::sqrt(2.0) / 2.0;
    const opencagd::curve::NURBSCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
        {1.0, w, 1.0});
    const auto samples = opencagd::curve::sample_curve(curve, 301);

    opencagd::visualization::PlotOptions options;
    options.title = "Quadratic NURBS exact quarter circle";
    options.output_path = "outputs/nurbs_quarter_circle.png";
    options.equal_axis = true;
    opencagd::visualization::plot_curve_2d(samples, curve.control_points(), options);
}

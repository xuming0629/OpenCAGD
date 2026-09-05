#include <opencagd/curve/bezier_curve.hpp>
#include <opencagd/curve/sampling.hpp>
#include <opencagd/visualization/matplotlib.hpp>

int main()
{
    const opencagd::curve::BezierCurve2d curve({
        {0.0, 0.0}, {1.0, 2.0}, {3.0, 3.0}, {4.0, 0.0}
    });
    const auto samples = opencagd::curve::sample_parameter_range(
        [&curve](double t) { return curve.evaluate(t); }, 0.0, 1.0, 301);

    opencagd::visualization::PlotOptions options;
    options.title = "Cubic Bezier curve";
    options.output_path = "outputs/bezier_curve.png";
    options.equal_axis = true;
    opencagd::visualization::plot_curve_2d(samples, curve.control_points(), options);
}

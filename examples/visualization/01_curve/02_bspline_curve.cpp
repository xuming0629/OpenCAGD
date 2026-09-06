#include <opencagd/curve/bspline_curve.hpp>
#include <opencagd/curve/sampling.hpp>
#include <opencagd/visualization/matplotlib.hpp>

#include <string>

int main(int argc, char** argv)
{
    using namespace opencagd;
    curve::BSplineCurve2d spline(
        3,
        {0,0,0,0,1,2,3,4,4,4,4},
        {{0,0},{1,2},{2,1},{3,3},{4,1},{5,2},{6,0}});
    const auto samples = curve::sample_curve(spline, 301);

    visualization::PlotOptions options;
    options.title = "Cubic B-spline Curve";
    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "bspline_curve.png";
    }
    visualization::plot_curve_2d(samples, spline.control_points(), options);
}

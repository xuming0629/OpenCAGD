#include <opencagd/curve/bezier_curve.hpp>
#include <opencagd/curve/sampling.hpp>
#include <opencagd/visualization/matplotlib.hpp>

#include <string>

int main(int argc, char** argv)
{
    using namespace opencagd;
    curve::BezierCurve2d bezier({{0.0,0.0},{1.0,2.5},{3.0,2.0},{4.0,0.0}});
    const auto samples = curve::sample_curve(bezier, 201);

    visualization::PlotOptions options;
    options.title = "Cubic Bezier Curve";
    options.xlabel = "x";
    options.ylabel = "y";
    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "bezier_curve.png";
    }
    visualization::plot_curve_2d(samples, bezier.control_points(), options);
}

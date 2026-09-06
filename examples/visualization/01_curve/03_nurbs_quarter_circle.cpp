#include <opencagd/curve/nurbs_curve.hpp>
#include <opencagd/curve/sampling.hpp>
#include <opencagd/visualization/matplotlib.hpp>

#include <cmath>
#include <string>

int main(int argc, char** argv)
{
    using namespace opencagd;
    const double w = 1.0 / std::sqrt(2.0);
    curve::NURBSCurve2d arc(
        2,
        {0,0,0,1,1,1},
        {{1,0},{1,1},{0,1}},
        {1,w,1});
    const auto samples = curve::sample_curve(arc, 201);

    visualization::PlotOptions options;
    options.title = "Exact NURBS Quarter Circle";
    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "nurbs_quarter_circle.png";
    }
    visualization::plot_curve_2d(samples, arc.control_points(), options);
}

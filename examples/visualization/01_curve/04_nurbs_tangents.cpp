#include <cmath>
#include <string>
#include <vector>

#include <opencagd/curve/nurbs_curve.hpp>
#include <opencagd/curve/sampling.hpp>
#include <opencagd/visualization/matplotlib_figure.hpp>

int main(int argc, char** argv)
{
    using namespace opencagd;

    const double w = std::sqrt(2.0) / 2.0;
    const curve::NURBSCurve2d arc(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
        {1.0, w, 1.0});

    const auto samples = curve::sample_curve(arc, 201);

    std::vector<Point2d> origins;
    std::vector<Point2d> tangents;

    for (std::size_t i = 0; i <= 8; ++i)
    {
        const double u = static_cast<double>(i) / 8.0;
        origins.push_back(arc.evaluate(u));
        tangents.push_back(arc.tangent(u));
    }

    visualization::PlotOptions options;
    options.title = "NURBS Quarter Circle: Unit Tangents";
    options.xlabel = "x";
    options.ylabel = "y";

    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "nurbs_quarter_circle_tangents.png";
    }

    visualization::MatplotlibFigure fig(
        visualization::MatplotlibFigure::AxisMode::TwoD,
        options);

    fig.add_curve_2d(samples, "NURBS curve");
    fig.add_control_polygon(arc.control_points());
    fig.add_vectors_2d(origins, tangents, 0.18, "unit tangents");
    fig.render();
}

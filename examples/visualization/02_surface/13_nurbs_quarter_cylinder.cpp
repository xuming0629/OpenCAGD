#include <opencagd/surface/nurbs_surface.hpp>
#include <opencagd/surface/sampling.hpp>
#include <opencagd/visualization/matplotlib_figure.hpp>

#include <cmath>
#include <string>

int main(int argc, char** argv)
{
    using namespace opencagd;
    const double w = 1.0 / std::sqrt(2.0);

    // Exact quarter cylinder: a quadratic NURBS quarter circle in u
    // tensor-product with a linear segment in v.
    surface::NURBSSurface3d cylinder(
        2, 1,
        {0,0,0,1,1,1},
        {0,0,1,1},
        {
            {{1,0,0},{1,0,2}},
            {{1,1,0},{1,1,2}},
            {{0,1,0},{0,1,2}}
        },
        {
            {1,1},
            {w,w},
            {1,1}
        });

    const auto grid = surface::sample_surface(cylinder, 61, 31);

    visualization::PlotOptions options;
    options.title = "Exact NURBS Quarter Cylinder";
    options.xlabel = "x";
    options.ylabel = "y";
    options.zlabel = "z";
    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "nurbs_quarter_cylinder.png";
    }

    visualization::MatplotlibFigure fig(
        visualization::MatplotlibFigure::AxisMode::ThreeD,
        options);
    fig.add_surface(grid);
    fig.add_isocurves(grid, 10, 10);
    fig.add_control_net(cylinder.control_points());
    fig.render();
}

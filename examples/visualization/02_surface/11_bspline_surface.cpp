#include <opencagd/surface/bspline_surface.hpp>
#include <opencagd/surface/sampling.hpp>
#include <opencagd/visualization/matplotlib_figure.hpp>

#include <string>

int main(int argc, char** argv)
{
    using namespace opencagd;
    surface::BSplineSurface3d patch(
        2, 2,
        {0,0,0,1,2,2,2},
        {0,0,0,1,2,2,2},
        {
            {{0,0,0},{0,1,0.3},{0,2,0.1},{0,3,0}},
            {{1,0,0.2},{1,1,1.1},{1,2,1.4},{1,3,0.2}},
            {{2,0,0.0},{2,1,1.5},{2,2,1.0},{2,3,0.4}},
            {{3,0,0},{3,1,0.1},{3,2,0.2},{3,3,0}}
        });

    const auto grid = surface::sample_surface(patch, 61, 61);
    visualization::PlotOptions options;
    options.title = "Tensor-product B-spline Surface";
    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "bspline_surface.png";
    }
    visualization::MatplotlibFigure fig(visualization::MatplotlibFigure::AxisMode::ThreeD, options);
    fig.add_surface(grid);
    fig.add_isocurves(grid, 12, 12);
    fig.add_control_net(patch.control_points());
    fig.render();
}

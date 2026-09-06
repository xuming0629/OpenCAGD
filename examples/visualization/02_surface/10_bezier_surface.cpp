#include <opencagd/surface/bezier_surface.hpp>
#include <opencagd/surface/sampling.hpp>
#include <opencagd/visualization/matplotlib_figure.hpp>

#include <string>

int main(int argc, char** argv)
{
    using namespace opencagd;
    surface::BezierSurface3d patch({
        {{0,0,0},{0,1,0.2},{0,2,-0.1},{0,3,0}},
        {{1,0,0.3},{1,1,1.5},{1,2,1.2},{1,3,0.2}},
        {{2,0,-0.1},{2,1,1.0},{2,2,1.8},{2,3,0.4}},
        {{3,0,0},{3,1,0.2},{3,2,0.1},{3,3,0}}
    });

    const auto grid = surface::sample_surface(patch, 51, 51);
    visualization::PlotOptions options;
    options.title = "Bicubic Bezier Surface";
    options.xlabel = "x"; options.ylabel = "y"; options.zlabel = "z";
    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "bezier_surface.png";
    }

    visualization::MatplotlibFigure fig(visualization::MatplotlibFigure::AxisMode::ThreeD, options);
    fig.add_surface(grid);
    fig.add_isocurves(grid, 10, 10);
    fig.add_control_net(patch.control_points());
    fig.render();
}

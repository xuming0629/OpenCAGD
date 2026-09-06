#include <cmath>
#include <string>
#include <vector>

#include <opencagd/surface/differential_geometry.hpp>
#include <opencagd/surface/nurbs_surface.hpp>
#include <opencagd/surface/sampling.hpp>
#include <opencagd/visualization/matplotlib_figure.hpp>

int main(int argc, char** argv)
{
    using namespace opencagd;

    const double w = std::sqrt(2.0) / 2.0;

    const surface::NURBSSurface3d cylinder(
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

    std::vector<Point3d> origins;
    std::vector<Point3d> normals;

    for (std::size_t iu = 0; iu <= 6; ++iu)
    {
        for (std::size_t iv = 0; iv <= 4; ++iv)
        {
            const double u = static_cast<double>(iu) / 6.0;
            const double v = static_cast<double>(iv) / 4.0;
            const auto dg = surface::differential_geometry(cylinder, u, v);
            origins.push_back(dg.point);
            normals.push_back(dg.normal);
        }
    }

    visualization::PlotOptions options;
    options.title = "NURBS Quarter Cylinder: Surface Normals";
    options.xlabel = "x";
    options.ylabel = "y";
    options.zlabel = "z";

    if (argc > 1 && std::string(argv[1]) == "--save")
    {
        options.show = false;
        options.output_path = "nurbs_quarter_cylinder_normals.png";
    }

    visualization::MatplotlibFigure fig(
        visualization::MatplotlibFigure::AxisMode::ThreeD,
        options);

    fig.add_surface(grid);
    fig.add_isocurves(grid, 10, 10);
    fig.add_vectors_3d(origins, normals, 0.22, "unit normals");
    fig.render();
}

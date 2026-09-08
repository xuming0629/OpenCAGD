#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <opencagd/curve.h>
#include <opencagd/plot.h>
#include <opencagd/vtk.h>
#include <opencagd/visualization.h>

int main(int argc, char** argv)
{
    const std::string backend = argc > 1 ? argv[1] : "plot";

    std::unique_ptr<opencagd::visualization::Renderer> renderer;
    if (backend == "plot")
        renderer.reset(new opencagd::plot::MatplotlibRenderer());
    else if (backend == "vtk")
        renderer.reset(new opencagd::vtk::VTKRenderer());
    else
    {
        std::cerr << "Usage: opencagd_backend_switch [plot|vtk]\n";
        return 2;
    }

    using opencagd::geometry::Point3D;
    const double w = std::sqrt(2.0) / 2.0;
    const std::vector<Point3D> control_points{
        Point3D(1,0), Point3D(1,1), Point3D(0,1)};
    const opencagd::curve::NURBSCurve quarter_circle(
        2,
        std::vector<double>{0,0,0,1,1,1},
        control_points,
        std::vector<double>{1,w,1});

    const opencagd::visualization::Curve2DData data =
        opencagd::visualization::make_curve_2d_data(
            opencagd::curve::sample_curve(quarter_circle, 240),
            control_points,
            "NURBS quarter circle");

    opencagd::visualization::RenderOptions options;
    options.title = std::string("Runtime renderer switch: ") + renderer->name();
    options.xlabel = "x";
    options.ylabel = "y";
    options.show = OPENCAGD_EXAMPLE_SHOW != 0;
    options.output_path = std::string(OPENCAGD_EXAMPLE_OUTPUT_DIR) + "/" +
                          renderer->name() + "/10_backend_switch.png";

    renderer->render(data, options);
    return 0;
}

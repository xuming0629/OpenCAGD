#include <iomanip>
#include <iostream>

#include <opencagd/surface/bezier_surface.hpp>
#include <opencagd/surface/differential_geometry.hpp>

int main()
{
    using namespace opencagd;
    using namespace opencagd::surface;

    // 双线性平面 z=0。
    const BezierSurface3d plane({
        {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}}
    });

    const auto dg = differential_geometry(plane, 0.35, 0.60);

    std::cout << std::fixed << std::setprecision(8);
    std::cout << "Su = (" << dg.du[0] << ", " << dg.du[1] << ", " << dg.du[2] << ")\n";
    std::cout << "Sv = (" << dg.dv[0] << ", " << dg.dv[1] << ", " << dg.dv[2] << ")\n";
    std::cout << "N  = (" << dg.normal[0] << ", " << dg.normal[1] << ", " << dg.normal[2] << ")\n";
    std::cout << "Gaussian curvature = " << dg.gaussian_curvature << '\n';
    std::cout << "Mean curvature     = " << dg.mean_curvature << '\n';
}

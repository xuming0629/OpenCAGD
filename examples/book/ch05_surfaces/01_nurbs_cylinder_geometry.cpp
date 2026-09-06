#include <cmath>
#include <iomanip>
#include <iostream>

#include <opencagd/surface/differential_geometry.hpp>
#include <opencagd/surface/nurbs_surface.hpp>

int main()
{
    using namespace opencagd::surface;

    const double w = std::sqrt(2.0) / 2.0;

    // 四分之一单位圆柱：圆弧方向为 u，直线方向为 v。
    const NURBSSurface3d cylinder(
        2,
        1,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {0.0, 0.0, 1.0, 1.0},
        {
            {{1.0, 0.0, 0.0}, {1.0, 0.0, 2.0}},
            {{1.0, 1.0, 0.0}, {1.0, 1.0, 2.0}},
            {{0.0, 1.0, 0.0}, {0.0, 1.0, 2.0}}
        },
        {
            {1.0, 1.0},
            {w, w},
            {1.0, 1.0}
        });

    const auto dg = differential_geometry(cylinder, 0.5, 0.4);

    std::cout << std::fixed << std::setprecision(10);
    std::cout << "point = (" << dg.point[0] << ", " << dg.point[1] << ", " << dg.point[2] << ")\n";
    std::cout << "normal = (" << dg.normal[0] << ", " << dg.normal[1] << ", " << dg.normal[2] << ")\n";
    std::cout << "K = " << dg.gaussian_curvature << "  (cylinder exact = 0)\n";
    std::cout << "H = " << dg.mean_curvature << "  (|exact| = 0.5 for radius 1)\n";
    std::cout << "principal = [" << dg.principal_curvature_min
              << ", " << dg.principal_curvature_max << "]\n";
}

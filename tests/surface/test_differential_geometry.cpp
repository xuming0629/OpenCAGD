#include <algorithm>
#include <cmath>

#include <gtest/gtest.h>

#include <opencagd/surface/bezier_surface.hpp>
#include <opencagd/surface/differential_geometry.hpp>
#include <opencagd/surface/nurbs_surface.hpp>

TEST(OpenCAGDSurfaceDifferentialGeometry, PlaneHasZeroCurvature)
{
    const opencagd::surface::BezierSurface3d plane({
        {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}}
    });

    const auto dg =
        opencagd::surface::differential_geometry(plane, 0.37, 0.61);

    EXPECT_NEAR(dg.normal[0], 0.0, 1.0e-12);
    EXPECT_NEAR(dg.normal[1], 0.0, 1.0e-12);
    EXPECT_NEAR(dg.normal[2], 1.0, 1.0e-12);

    EXPECT_NEAR(dg.gaussian_curvature, 0.0, 1.0e-12);
    EXPECT_NEAR(dg.mean_curvature, 0.0, 1.0e-12);
}

TEST(OpenCAGDSurfaceDifferentialGeometry, UnitCylinderHasExpectedCurvatures)
{
    const double w = std::sqrt(2.0) / 2.0;

    const opencagd::surface::NURBSSurface3d cylinder(
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

    const auto dg =
        opencagd::surface::differential_geometry(cylinder, 0.5, 0.4);

    EXPECT_NEAR(dg.gaussian_curvature, 0.0, 2.0e-12);
    EXPECT_NEAR(std::abs(dg.mean_curvature), 0.5, 2.0e-12);

    const double k_abs_max = std::max(
        std::abs(dg.principal_curvature_min),
        std::abs(dg.principal_curvature_max));
    const double k_abs_min = std::min(
        std::abs(dg.principal_curvature_min),
        std::abs(dg.principal_curvature_max));

    EXPECT_NEAR(k_abs_max, 1.0, 2.0e-12);
    EXPECT_NEAR(k_abs_min, 0.0, 2.0e-12);
}

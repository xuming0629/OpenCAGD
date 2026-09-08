#include <cmath>
#include <gtest/gtest.h>
#include <opencagd/surface.h>

namespace {
using opencagd::geometry::Point3D;

TEST(BezierSurfaceTest, PlaneHasConstantNormalAndZeroCurvature)
{
    const opencagd::surface::BezierSurface::ControlNet plane{
        {Point3D(0,0,0), Point3D(0,1,0)},
        {Point3D(1,0,0), Point3D(1,1,0)}};
    const opencagd::surface::BezierSurface surface(plane);
    const auto n = surface.normal(0.5, 0.5);
    EXPECT_NEAR(n.x(), 0.0, 1e-12); EXPECT_NEAR(n.y(), 0.0, 1e-12); EXPECT_NEAR(n.z(), 1.0, 1e-12);
    const auto dg = opencagd::surface::differential_geometry(surface, 0.5, 0.5);
    EXPECT_NEAR(dg.gaussian_curvature, 0.0, 1e-12);
    EXPECT_NEAR(dg.mean_curvature, 0.0, 1e-12);
}

TEST(NURBSSurfaceTest, QuarterCylinderGeometryIsExact)
{
    const double w = std::sqrt(0.5), s = std::sqrt(0.5);
    const opencagd::surface::NURBSSurface::ControlNet control{
        {Point3D(1,0,0), Point3D(1,0,1)},
        {Point3D(1,1,0), Point3D(1,1,1)},
        {Point3D(0,1,0), Point3D(0,1,1)}};
    const opencagd::surface::NURBSSurface::WeightNet weights{{1,1},{w,w},{1,1}};
    const opencagd::surface::NURBSSurface surface(
        2, {0,0,0,1,1,1}, 1, {0,0,1,1}, control, weights);
    const Point3D p = surface.evaluate(0.5, 0.5);
    EXPECT_NEAR(p.x(), s, 1e-10); EXPECT_NEAR(p.y(), s, 1e-10); EXPECT_NEAR(p.z(), 0.5, 1e-12);
    const auto n = surface.normal(0.5, 0.5);
    EXPECT_NEAR(n.x(), s, 1e-10); EXPECT_NEAR(n.y(), s, 1e-10); EXPECT_NEAR(n.z(), 0.0, 1e-10);
    const auto dg = opencagd::surface::differential_geometry(surface, 0.5, 0.5);
    EXPECT_NEAR(dg.gaussian_curvature, 0.0, 1e-9);
}
} // namespace

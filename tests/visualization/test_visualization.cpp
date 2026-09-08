#include <gtest/gtest.h>
#include <opencagd/surface.h>
#include <opencagd/visualization.h>

TEST(VisualizationBuilderTest, BuildsNormalGlyphsFromSurface)
{
    using opencagd::geometry::Point3D;
    const opencagd::surface::BezierSurface::ControlNet plane{
        {Point3D(0,0,0),Point3D(0,1,0)}, {Point3D(1,0,0),Point3D(1,1,0)}};
    const opencagd::surface::BezierSurface surface(plane);
    const auto grid = opencagd::surface::sample_surface(surface,5,5);
    auto data = opencagd::visualization::make_surface_3d_data(grid,plane);
    opencagd::visualization::append_surface_normals(data,surface,3,4,0.2);
    ASSERT_EQ(data.vectors.size(),12u);
    for (const auto& glyph : data.vectors) {
        EXPECT_NEAR(glyph.direction.x(),0.0,1e-12);
        EXPECT_NEAR(glyph.direction.y(),0.0,1e-12);
        EXPECT_NEAR(glyph.direction.z(),1.0,1e-12);
        EXPECT_NEAR(glyph.scale,0.2,1e-15);
    }
}

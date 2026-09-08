#include <gtest/gtest.h>
#include <opencagd/vtk.h>

TEST(VTKRendererTest, ReportsBackendName)
{
    opencagd::vtk::VTKRenderer renderer;
    EXPECT_EQ(renderer.name(), "vtk");
}

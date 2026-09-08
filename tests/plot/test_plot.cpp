#include <fstream>
#include <gtest/gtest.h>
#include <opencagd/plot.h>
#include <opencagd/visualization.h>

TEST(MatplotlibRendererTest, RendersPngWithoutInteractiveWindow)
{
    using opencagd::geometry::Point3D;
    const std::vector<Point3D> points{Point3D(0,0),Point3D(0.5,1),Point3D(1,0)};
    const auto data=opencagd::visualization::make_curve_2d_data(points,{},"curve");
    opencagd::visualization::RenderOptions options;
    options.show=false;
    options.output_path=std::string(OPENCAGD_TEST_OUTPUT_DIR)+"/plot_smoke.png";
    opencagd::plot::MatplotlibRenderer renderer;
    EXPECT_EQ(renderer.name(),"matplotlib");
    ASSERT_NO_THROW(renderer.render(data,options));
    std::ifstream file(options.output_path.c_str(),std::ios::binary);
    EXPECT_TRUE(file.good());
}

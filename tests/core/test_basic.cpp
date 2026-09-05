#include <string>
#include <string_view>

#include <gtest/gtest.h>
#include <opencagd/opencagd.hpp>

TEST(OpenCAGDVersion, GeneratedVersionIsConsistent)
{
    const std::string expected =
        std::to_string(opencagd::version_major) + "." +
        std::to_string(opencagd::version_minor) + "." +
        std::to_string(opencagd::version_patch);

    EXPECT_EQ(std::string_view(opencagd::version), expected);
}

TEST(OpenCAGDPoint3d, StoresCoordinates)
{
    const opencagd::Point3d p{1.0, 2.0, 3.0};

    EXPECT_DOUBLE_EQ(p[0], 1.0);
    EXPECT_DOUBLE_EQ(p[1], 2.0);
    EXPECT_DOUBLE_EQ(p[2], 3.0);
}

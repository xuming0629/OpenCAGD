#include <iostream>

#include <opencagd/opencagd.hpp>

int main()
{
    const opencagd::Point3d point{1.0, 2.0, 3.0};

    std::cout << "OpenCAGD " << opencagd::version << '\n'
              << "Point3d = ("
              << point[0] << ", "
              << point[1] << ", "
              << point[2] << ")\n";

    return 0;
}

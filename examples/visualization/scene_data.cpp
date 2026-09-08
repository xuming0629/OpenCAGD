#include <iostream>
#include <opencagd/surface.h>
#include <opencagd/visualization.h>

int main()
{
    using opencagd::geometry::Point3D;
    const opencagd::surface::BezierSurface::ControlNet net{
        {Point3D(0,0,0), Point3D(0,1,0)},
        {Point3D(1,0,0), Point3D(1,1,0)}};
    const opencagd::surface::BezierSurface surface(net);
    const auto grid = opencagd::surface::sample_surface(surface, 8, 6);
    auto data = opencagd::visualization::make_surface_3d_data(grid, net);
    opencagd::visualization::append_surface_normals(data, surface, 3, 3, 0.15);
    std::cout << "backend-neutral surface scene\n"
              << "grid points: " << data.grid.points().size() << '\n'
              << "normal glyphs: " << data.vectors.size() << '\n';
    return 0;
}

#include <opencagd/geometry/point3d.h>

#include <stdexcept>

namespace opencagd {
namespace geometry {

Point3D::Point3D() : data_{0.0, 0.0, 0.0} {}
Point3D::Point3D(double x, double y, double z) : data_{x, y, z} {}

double Point3D::x() const { return data_[0]; }
double Point3D::y() const { return data_[1]; }
double Point3D::z() const { return data_[2]; }

double Point3D::operator[](std::size_t index) const
{
    if (index >= 3) throw std::out_of_range("Point3D index out of range");
    return data_[index];
}

double& Point3D::operator[](std::size_t index)
{
    if (index >= 3) throw std::out_of_range("Point3D index out of range");
    return data_[index];
}

Vector3D Point3D::as_vector() const
{
    return Vector3D(data_[0], data_[1], data_[2]);
}

Point3D operator+(const Point3D& point, const Vector3D& vector)
{
    return Point3D(point.x() + vector.x(), point.y() + vector.y(), point.z() + vector.z());
}

Point3D operator-(const Point3D& point, const Vector3D& vector)
{
    return Point3D(point.x() - vector.x(), point.y() - vector.y(), point.z() - vector.z());
}

Vector3D operator-(const Point3D& a, const Point3D& b)
{
    return Vector3D(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

Point3D lerp(const Point3D& a, const Point3D& b, double t)
{
    return a + (b - a) * t;
}

} // namespace geometry
} // namespace opencagd

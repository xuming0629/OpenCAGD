#include <iostream>
#include <opencagd/core.h>
int main() {
    using namespace opencagd::geometry;
    const Point3D p(1,2,3), q(4,2,1);
    const Vector3D pq=q-p, z=cross(Vector3D(1,0,0),Vector3D(0,1,0));
    std::cout << "P->Q = ("<<pq.x()<<", "<<pq.y()<<", "<<pq.z()<<")\n";
    std::cout << "ex x ey = ("<<z.x()<<", "<<z.y()<<", "<<z.z()<<")\n";
    return 0;
}

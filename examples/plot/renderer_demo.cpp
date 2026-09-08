#include <vector>
#include <opencagd/curve.h>
#include <opencagd/plot.h>
#include <opencagd/visualization.h>
int main(){using opencagd::geometry::Point3D;opencagd::curve::BezierCurve c({Point3D(0,0),Point3D(1,2),Point3D(3,2),Point3D(4,0)});auto points=opencagd::curve::sample_curve(c,101);auto data=opencagd::visualization::make_curve_2d_data(points,c.control_points(),"Bezier curve");opencagd::visualization::RenderOptions o;o.title="OpenCAGD Plot backend";o.show=OPENCAGD_EXAMPLE_SHOW!=0;o.output_path=std::string(OPENCAGD_EXAMPLE_OUTPUT_DIR)+"/renderer_demo.png";opencagd::plot::MatplotlibRenderer().render(data,o);return 0;}

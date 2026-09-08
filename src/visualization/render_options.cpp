#include <opencagd/visualization/render_options.h>

namespace opencagd {
namespace visualization {

RenderOptions::RenderOptions()
    : title(), xlabel(), ylabel(), zlabel(), output_path(),
      grid(true), equal_axis(true), show(true), width(900), height(680)
{
}

} // namespace visualization
} // namespace opencagd

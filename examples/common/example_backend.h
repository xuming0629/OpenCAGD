#pragma once

#include <string>
#include <opencagd/visualization.h>

#if defined(OPENCAGD_EXAMPLE_BACKEND_PLOT)
  #include <opencagd/plot.h>
  namespace opencagd_example {
  typedef opencagd::plot::MatplotlibRenderer Renderer;
  inline const char* backend_name() { return "plot"; }
  }
#elif defined(OPENCAGD_EXAMPLE_BACKEND_VTK)
  #include <opencagd/vtk.h>
  namespace opencagd_example {
  typedef opencagd::vtk::VTKRenderer Renderer;
  inline const char* backend_name() { return "vtk"; }
  }
#else
  #error "An OpenCAGD example renderer backend must be selected"
#endif

namespace opencagd_example {
inline std::string output_path(const std::string& filename)
{
    return std::string(OPENCAGD_EXAMPLE_OUTPUT_DIR) + "/" + backend_name() + "/" + filename;
}

inline void finalize_options(opencagd::visualization::RenderOptions& options,
                             const std::string& filename)
{
    options.output_path = output_path(filename);
    options.show = OPENCAGD_EXAMPLE_SHOW != 0;
}
}

#pragma once

#include <opencagd/core/export.h>
#include <opencagd/visualization/renderer.h>

namespace opencagd {
namespace vtk {

class OPENCAGD_VTK_API VTKRenderer : public visualization::Renderer
{
public:
    VTKRenderer();
    ~VTKRenderer() override;

    std::string name() const override;
    void render(const visualization::Curve2DData& data,
                const visualization::RenderOptions& options) override;
    void render(const visualization::BasisFunctionData& data,
                const visualization::RenderOptions& options) override;
    void render(const visualization::DeCasteljauData& data,
                const visualization::RenderOptions& options) override;
    void render(const visualization::Surface3DData& data,
                const visualization::RenderOptions& options) override;
};

} // namespace vtk
} // namespace opencagd

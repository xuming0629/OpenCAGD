#pragma once

#include <string>
#include <opencagd/core/export.h>
#include <opencagd/visualization/data.h>
#include <opencagd/visualization/render_options.h>

namespace opencagd {
namespace visualization {

class OPENCAGD_VISUALIZATION_API Renderer
{
public:
    virtual ~Renderer();

    virtual std::string name() const = 0;
    virtual void render(const Curve2DData& data, const RenderOptions& options) = 0;
    virtual void render(const BasisFunctionData& data, const RenderOptions& options) = 0;
    virtual void render(const DeCasteljauData& data, const RenderOptions& options) = 0;
    virtual void render(const Surface3DData& data, const RenderOptions& options) = 0;
};

} // namespace visualization
} // namespace opencagd

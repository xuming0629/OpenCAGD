#include <opencagd/vtk/vtk_renderer.h>

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkCubeAxesActor.h>
#include <vtkCellArray.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLine.h>
#include <vtkPNGWriter.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkWindowToImageFilter.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

namespace opencagd {
namespace vtk {
namespace {

vtkSmartPointer<vtkActor> make_polyline_actor(
    const std::vector<geometry::Point3D>& points,
    double line_width,
    bool show_points)
{
    vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
    for (std::size_t i = 0; i < points.size(); ++i)
        vtk_points->InsertNextPoint(points[i].x(), points[i].y(), points[i].z());

    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    if (points.size() >= 2)
    {
        lines->InsertNextCell(static_cast<vtkIdType>(points.size()));
        for (std::size_t i = 0; i < points.size(); ++i)
            lines->InsertCellPoint(static_cast<vtkIdType>(i));
    }

    vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    poly->SetPoints(vtk_points);
    poly->SetLines(lines);

    if (show_points)
    {
        vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();
        for (std::size_t i = 0; i < points.size(); ++i)
        {
            verts->InsertNextCell(1);
            verts->InsertCellPoint(static_cast<vtkIdType>(i));
        }
        poly->SetVerts(verts);
    }

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(line_width);
    actor->GetProperty()->SetPointSize(show_points ? 7.0 : 1.0);
    return actor;
}

vtkSmartPointer<vtkActor> make_surface_actor(const visualization::Surface3DData& data)
{
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    const std::vector<geometry::Point3D>& source = data.grid.points();
    for (std::size_t i = 0; i < source.size(); ++i)
        points->InsertNextPoint(source[i].x(), source[i].y(), source[i].z());

    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    const std::size_t nu = data.grid.u_count();
    const std::size_t nv = data.grid.v_count();
    for (std::size_t i = 0; i + 1 < nu; ++i)
    {
        for (std::size_t j = 0; j + 1 < nv; ++j)
        {
            vtkIdType ids[4] = {
                static_cast<vtkIdType>(i * nv + j),
                static_cast<vtkIdType>((i + 1) * nv + j),
                static_cast<vtkIdType>((i + 1) * nv + (j + 1)),
                static_cast<vtkIdType>(i * nv + (j + 1))};
            polys->InsertNextCell(4, ids);
        }
    }

    vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    poly->SetPoints(points);
    poly->SetPolys(polys);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.82);
    actor->GetProperty()->SetColor(0.55, 0.72, 0.90);
    return actor;
}

vtkSmartPointer<vtkActor> make_vector_actor(const std::vector<visualization::VectorGlyph3D>& vectors)
{
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkDoubleArray> vec = vtkSmartPointer<vtkDoubleArray>::New();
    vec->SetNumberOfComponents(3);
    vec->SetName("vectors");
    for (std::size_t i = 0; i < vectors.size(); ++i)
    {
        const visualization::VectorGlyph3D& g = vectors[i];
        points->InsertNextPoint(g.origin.x(), g.origin.y(), g.origin.z());
        const double tuple[3] = {
            g.direction.x() * g.scale,
            g.direction.y() * g.scale,
            g.direction.z() * g.scale};
        vec->InsertNextTuple(tuple);
    }
    vtkSmartPointer<vtkPolyData> data = vtkSmartPointer<vtkPolyData>::New();
    data->SetPoints(points);
    data->GetPointData()->SetVectors(vec);

    vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputData(data);
    glyph->SetSourceConnection(arrow->GetOutputPort());
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToScaleByVector();
    glyph->OrientOn();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0.78, 0.20, 0.18);
    return actor;
}

void finish_scene(vtkRenderer* renderer,
                  const visualization::RenderOptions& options,
                  bool two_dimensional)
{
    renderer->SetBackground(0.96, 0.97, 0.98);

    double bounds[6] = {0,1,0,1,0,1};
    renderer->ComputeVisiblePropBounds(bounds);

    vtkSmartPointer<vtkCubeAxesActor> axes = vtkSmartPointer<vtkCubeAxesActor>::New();
    axes->SetBounds(bounds);
    axes->SetCamera(renderer->GetActiveCamera());
    axes->SetXTitle(options.xlabel.empty() ? "x" : options.xlabel.c_str());
    axes->SetYTitle(options.ylabel.empty() ? "y" : options.ylabel.c_str());
    axes->SetZTitle(options.zlabel.empty() ? "z" : options.zlabel.c_str());
    if (two_dimensional) axes->SetZAxisVisibility(false);
    if (options.grid)
    {
        axes->DrawXGridlinesOn();
        axes->DrawYGridlinesOn();
        if (!two_dimensional) axes->DrawZGridlinesOn();
    }
    renderer->AddActor(axes);

    if (!options.title.empty())
    {
        vtkSmartPointer<vtkTextActor> title = vtkSmartPointer<vtkTextActor>::New();
        title->SetInput(options.title.c_str());
        title->SetPosition(18.0, static_cast<double>(options.height - 32));
        title->GetTextProperty()->SetFontSize(18);
        title->GetTextProperty()->SetColor(0.12, 0.12, 0.12);
        renderer->AddActor2D(title);
    }

    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);
    window->SetSize(options.width, options.height);
    window->SetWindowName(options.title.empty() ? "OpenCAGD VTK" : options.title.c_str());
    if (!options.show)
        window->SetOffScreenRendering(1);

    renderer->ResetCamera();
    if (two_dimensional)
    {
        renderer->GetActiveCamera()->ParallelProjectionOn();
        renderer->GetActiveCamera()->SetPosition(0.0, 0.0, 10.0);
        renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
        renderer->GetActiveCamera()->SetViewUp(0.0, 1.0, 0.0);
        renderer->ResetCamera();
    }
    window->Render();

    if (!options.output_path.empty())
    {
        vtkSmartPointer<vtkWindowToImageFilter> capture = vtkSmartPointer<vtkWindowToImageFilter>::New();
        capture->SetInput(window);
        capture->SetInputBufferTypeToRGBA();
        capture->ReadFrontBufferOff();
        capture->Update();
        vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
        writer->SetFileName(options.output_path.c_str());
        writer->SetInputConnection(capture->GetOutputPort());
        writer->Write();
    }

    if (options.show)
    {
        vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        interactor->SetInteractorStyle(style);
        interactor->SetRenderWindow(window);
        interactor->Initialize();
        interactor->Start();
    }
}

void add_control_net(vtkRenderer* renderer,
                     const std::vector<std::vector<geometry::Point3D> >& net)
{
    if (net.empty()) return;
    const std::size_t rows = net.size();
    const std::size_t cols = net[0].size();
    for (std::size_t i = 0; i < rows; ++i)
        renderer->AddActor(make_polyline_actor(net[i], 1.5, true));
    for (std::size_t j = 0; j < cols; ++j)
    {
        std::vector<geometry::Point3D> column;
        for (std::size_t i = 0; i < rows; ++i) column.push_back(net[i][j]);
        renderer->AddActor(make_polyline_actor(column, 1.5, true));
    }
}
}

VTKRenderer::VTKRenderer() {}
VTKRenderer::~VTKRenderer() {}
std::string VTKRenderer::name() const { return "vtk"; }

void VTKRenderer::render(const visualization::Curve2DData& data,
                         const visualization::RenderOptions& options)
{
    if (data.curves.empty()) throw std::invalid_argument("curve data must not be empty");
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    const double palette[4][3] = {{0.16,0.38,0.66},{0.18,0.58,0.36},{0.70,0.30,0.58},{0.58,0.42,0.18}};
    for (std::size_t i = 0; i < data.curves.size(); ++i)
    {
        vtkSmartPointer<vtkActor> actor = make_polyline_actor(data.curves[i].points, 3.0, false);
        actor->GetProperty()->SetColor(palette[i % 4][0], palette[i % 4][1], palette[i % 4][2]);
        renderer->AddActor(actor);
    }
    if (!data.control_points.empty())
    {
        vtkSmartPointer<vtkActor> control = make_polyline_actor(data.control_points, 1.5, true);
        control->GetProperty()->SetColor(0.80, 0.35, 0.12);
        renderer->AddActor(control);
    }
    finish_scene(renderer, options, true);
}

void VTKRenderer::render(const visualization::BasisFunctionData& data,
                         const visualization::RenderOptions& options)
{
    if (data.parameters.empty() || data.values.empty())
        throw std::invalid_argument("basis data must not be empty");
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    for (std::size_t s = 0; s < data.values.size(); ++s)
    {
        if (data.values[s].size() != data.parameters.size())
            throw std::invalid_argument("basis series length mismatch");
        std::vector<geometry::Point3D> points;
        for (std::size_t i = 0; i < data.parameters.size(); ++i)
            points.push_back(geometry::Point3D(data.parameters[i], data.values[s][i], 0.0));
        renderer->AddActor(make_polyline_actor(points, 2.0, false));
    }
    finish_scene(renderer, options, true);
}

void VTKRenderer::render(const visualization::DeCasteljauData& data,
                         const visualization::RenderOptions& options)
{
    if (data.levels.empty()) throw std::invalid_argument("De Casteljau data must not be empty");
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    for (std::size_t i = 0; i < data.levels.size(); ++i)
        renderer->AddActor(make_polyline_actor(data.levels[i], i == 0 ? 2.0 : 1.4, true));
    finish_scene(renderer, options, true);
}

void VTKRenderer::render(const visualization::Surface3DData& data,
                         const visualization::RenderOptions& options)
{
    if (data.grid.empty()) throw std::invalid_argument("surface data must not be empty");
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(make_surface_actor(data));
    add_control_net(renderer, data.control_net);
    if (!data.vectors.empty())
        renderer->AddActor(make_vector_actor(data.vectors));
    finish_scene(renderer, options, false);
}

} // namespace vtk
} // namespace opencagd

#include "python_runtime.h"

#include <Python.h>
#include <mutex>
#include <stdexcept>

namespace opencagd {
namespace plot {
namespace {
std::mutex& python_mutex()
{
    static std::mutex mutex;
    return mutex;
}
}

PythonRuntime& PythonRuntime::instance()
{
    static PythonRuntime runtime;
    return runtime;
}

PythonRuntime::PythonRuntime()
{
    std::lock_guard<std::mutex> lock(python_mutex());
    if (!Py_IsInitialized())
        Py_Initialize();
}

PythonRuntime::~PythonRuntime()
{
    // Deliberately do not call Py_Finalize here. OpenCAGD may be embedded in a
    // larger host process that owns Python's lifetime.
}

void PythonRuntime::execute(const std::string& code)
{
    std::lock_guard<std::mutex> lock(python_mutex());
    if (!Py_IsInitialized())
        Py_Initialize();
    const int rc = PyRun_SimpleString(code.c_str());
    if (rc != 0)
        throw std::runtime_error("Python/Matplotlib rendering failed");
}

} // namespace plot
} // namespace opencagd

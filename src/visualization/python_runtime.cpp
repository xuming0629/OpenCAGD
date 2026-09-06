#include <opencagd/visualization/python_runtime.hpp>

#include <Python.h>

#include <stdexcept>

namespace opencagd::visualization
{
PythonRuntime& PythonRuntime::instance()
{
    static PythonRuntime runtime;
    return runtime;
}

void PythonRuntime::execute(const std::string& script)
{
    std::lock_guard lock(mutex_);
    if (!Py_IsInitialized()) Py_Initialize();

    PyGILState_STATE gil = PyGILState_Ensure();
    const int rc = PyRun_SimpleString(script.c_str());
    if (rc != 0)
    {
        if (PyErr_Occurred()) PyErr_Print();
        PyGILState_Release(gil);
        throw std::runtime_error("Embedded Python/Matplotlib execution failed");
    }
    PyGILState_Release(gil);
}
} // namespace opencagd::visualization

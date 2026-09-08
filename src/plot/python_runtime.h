#pragma once

#include <string>

namespace opencagd {
namespace plot {

class PythonRuntime
{
public:
    static PythonRuntime& instance();
    void execute(const std::string& code);

private:
    PythonRuntime();
    ~PythonRuntime();
    PythonRuntime(const PythonRuntime&);
    PythonRuntime& operator=(const PythonRuntime&);
};

} // namespace plot
} // namespace opencagd

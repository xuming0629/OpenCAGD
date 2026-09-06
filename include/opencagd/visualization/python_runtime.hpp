#pragma once

#include <mutex>
#include <string>

namespace opencagd::visualization
{
class PythonRuntime
{
public:
    static PythonRuntime& instance();
    void execute(const std::string& script);

    PythonRuntime(const PythonRuntime&) = delete;
    PythonRuntime& operator=(const PythonRuntime&) = delete;

private:
    PythonRuntime() = default;
    ~PythonRuntime() = default;

    std::mutex mutex_;
};
} // namespace opencagd::visualization

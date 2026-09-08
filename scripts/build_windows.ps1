param(
    [ValidateSet("msvc-debug", "msvc-debug-plot", "msvc-debug-vtk")]
    [string]$Preset = "msvc-debug"
)

$ErrorActionPreference = "Stop"

cmake --preset $Preset
cmake --build --preset $Preset

if ($Preset -eq "msvc-debug") {
    ctest --preset msvc-debug
}

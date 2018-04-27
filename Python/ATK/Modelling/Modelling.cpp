/**
 * \file Modelling.cpp
 */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

using namespace ATK;

namespace
{
}

PYBIND11_MODULE(PythonModelling, m)
{
  m.doc() = "Audio ToolKit Modelling module";
}

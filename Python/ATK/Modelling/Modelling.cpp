/**
 * \file Modelling.cpp
 */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

namespace
{
}

PYBIND11_MODULE(PythonModelling, m)
{
  m.doc() = "Audio ToolKit Modelling module";
  
  py::object f1 = (py::object) py::module::import("ATK.Core").attr("FloatTypedBaseFilter");
  py::object f2 = (py::object) py::module::import("ATK.Core").attr("DoubleTypedBaseFilter");
}

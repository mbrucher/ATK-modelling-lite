/**
 * \file Modelling.cpp
 */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/SPICE/parser.h>

namespace py = pybind11;

namespace
{
}

PYBIND11_MODULE(PythonModelling, m)
{
  m.doc() = "Audio ToolKit Modelling module";
  
  py::object f2 = (py::object) py::module::import("ATK.Core").attr("DoubleTypedBaseFilter");
  
  py::class_<ATK::ast::SPICEAST>(m, "AST")
  .def(py::init())
  .def("parse_string", [](ATK::ast::SPICEAST& ast, const std::string& str) {
    ATK::parse_string(ast, str);
  })
  ;
  
  py::class_<ATK::ModellerFilter<double>>(m, "DoubleModellerFilter", f2)
  .def_static("create_dynamic_filter", [](const ATK::ast::SPICEAST& tree) {
    return ATK::SPICEHandler<double>::convert(tree);
  })
  ;
}

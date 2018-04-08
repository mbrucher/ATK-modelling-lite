/**
 * \file Modeling.cpp
 */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <ATK/Adaptive/BlockLMSFilter.h>
#include <ATK/Adaptive/LMSFilter.h>
#include <ATK/Adaptive/RLSFilter.h>

namespace py = pybind11;

using namespace ATK;

namespace
{
}

PYBIND11_MODULE(PythonModeling, m)
{
  m.doc() = "Audio ToolKit Modeling module";
}

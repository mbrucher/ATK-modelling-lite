/**
 * \file Diode.cpp
 */

#include "ModellerFilter.h"
#include "Diode.h"

#include <ATK/Utility/fmath.h>

namespace ATK
{
  template<typename DataType_, unsigned int direct, unsigned int indirect>
  Diode<DataType_, direct, indirect>::Diode(DataType Is, DataType N, DataType Vt)
  :Is(Is), N(N), Vt(Vt), precomp(0)
  {
  }

  template<typename DataType_, unsigned int direct, unsigned int indirect>
  typename Diode<DataType_, direct, indirect>::DataType Diode<DataType_, direct, indirect>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return Is * (direct * (precomp - 1) - indirect * (1 / precomp - 1)) * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_, unsigned int direct, unsigned int indirect>
  typename Diode<DataType_, direct, indirect>::DataType Diode<DataType_, direct, indirect>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    return Is / (N * Vt) * (direct * precomp + indirect / precomp) * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_, unsigned int direct, unsigned int indirect>
  void Diode<DataType_, direct, indirect>::precompute(bool steady_state)
  {
    precomp = fmath::exp((modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0])) / (N * Vt));
  }

  template class Diode<double, 1, 0>;
  template class Diode<double, 1, 1>;
  template class Diode<double, 2, 1>;
}

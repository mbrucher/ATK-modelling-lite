/**
 * \file Diode.cpp
 */

#include "ModellerFilter.h"
#include "Diode.h"

#include <ATK/Utility/fmath.h>

namespace ATK
{
  template<typename DataType_>
  Diode<DataType_>::Diode(DataType Is, DataType N, DataType Vt)
  :Is(Is), N(N), Vt(Vt), precomp(0)
  {
  }

  template<typename DataType_>
  typename Diode<DataType_>::DataType Diode<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return Is * precomp * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  typename Diode<DataType_>::DataType Diode<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    return Is / (N * Vt) * precomp * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  void Diode<DataType_>::precompute(bool steady_state)
  {
    precomp = fmath::exp((modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0])) / (N * Vt));
  }

  template class Diode<double>;
}

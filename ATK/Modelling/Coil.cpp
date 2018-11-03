/**
 * \file Coil.cpp
 */

#include "ModellerFilter.h"
#include "Coil.h"

namespace ATK
{
  template<typename DataType_>
  Coil<DataType_>::Coil(DataType_ L)
  :inner(L)
  {
  }

  template<typename DataType_>
  void Coil<DataType_>::update_steady_state(DataType dt)
  {
    Parent::update_steady_state(dt);
    inner.update_steady_state(dt);
  }
  
  template<typename DataType_>
  void Coil<DataType_>::update_state()
  {
    inner.update_state();
  }
  
  template<typename DataType_>
  DataType_ Coil<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return inner.get_current() * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  DataType_ Coil<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    return inner.get_gradient(steady_state)  * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  void Coil<DataType_>::precompute(bool steady_state)
  {
    return inner.precompute(modeller->retrieve_voltage(pins[0]), modeller->retrieve_voltage(pins[1]), steady_state);
  }
  
  template<typename DataType_>
  DataType_ Coil<DataType_>::get_coil() const
  {
    return inner.get_coil();
  }

  template class Coil<double>;
}

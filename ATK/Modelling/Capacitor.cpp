/**
 * \file Capacitor.cpp
 */

#include "ModellerFilter.h"
#include "Capacitor.h"

namespace ATK
{
  template<typename DataType_>
  Capacitor<DataType_>::Capacitor(DataType_ C)
  :inner(C)
  {
  }

  template<typename DataType_>
  void Capacitor<DataType_>::update_steady_state(DataType dt)
  {
    Parent::update_steady_state(dt);
    inner.update_steady_state(dt, modeller->retrieve_voltage(pins[0]), modeller->retrieve_voltage(pins[1]));
  }
  
  template<typename DataType_>
  void Capacitor<DataType_>::update_state()
  {
    inner.update_state(modeller->retrieve_voltage(pins[0]), modeller->retrieve_voltage(pins[1]));
  }
  
  template<typename DataType_>
  DataType_ Capacitor<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    if(steady_state)
    {
      return 0;
    }
    return inner.get_current(modeller->retrieve_voltage(pins[0]), modeller->retrieve_voltage(pins[1])) * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  DataType_ Capacitor<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    if(steady_state)
    {
      return 0;
    }
    return inner.get_gradient() * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }

  template<typename DataType_>
  DataType_ Capacitor<DataType_>::get_capacitance() const
  {
    return inner.get_capacitance();
  }

  template class Capacitor<double>;
}

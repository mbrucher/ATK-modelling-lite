/**
 * \file Resistor.cpp
 */

#include "DynamicModellerFilter.h"
#include "Resistor.h"

namespace ATK
{
  template<typename DataType_>
  Resistor<DataType_>::Resistor(DataType_ R)
  :inner(R)
  {
  }

  template<typename DataType_>
  DataType_ Resistor<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return inner.get_current(modeller->retrieve_voltage(pins[0]), modeller->retrieve_voltage(pins[1])) * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  DataType_ Resistor<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    return inner.get_gradient() * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  DataType_ Resistor<DataType_>::get_resistance() const
  {
    return inner.get_resistance();
  }
  
  template class Resistor<double>;
}

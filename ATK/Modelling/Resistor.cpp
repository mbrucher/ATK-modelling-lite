/**
 * \file Resistor.cpp
 */

#include "Modeler.h"
#include "Resistor.h"

namespace ATK
{
  template<typename DataType_>
  Resistor<DataType_>::Resistor(DataType_ R)
  :G(1/R)
  {
  }

  template<typename DataType_>
  typename Resistor<DataType_>::DataType Resistor<DataType_>::get_current(gsl::index pin_index, bool steady_state)
  {
    return (modeler->retrieve_voltage(pins[1]) - modeler->retrieve_voltage(pins[0])) * G * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  typename Resistor<DataType_>::DataType Resistor<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state)
  {
    return G * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }
}

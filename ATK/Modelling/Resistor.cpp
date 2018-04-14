/**
 * \file Resistor.cpp
 */

#include "Modeler.h"
#include "Resistor.h"

namespace ATK
{
  Resistor::Resistor(DataType R)
  :R(R), G(1/R)
  {
  }

  Resistor::DataType Resistor::get_current(gsl::index pin_index, bool steady_state)
  {
    return (modeler->retrieve_voltage(pins[1]) - modeler->retrieve_voltage(pins[0])) * G * (0 == pin_index ? 1 : -1);
  }
  
  Resistor::DataType Resistor::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state)
  {
    
  }
}

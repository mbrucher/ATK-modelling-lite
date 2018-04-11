/**
 * \file Modeler.cpp
 */

#include "Component.h"
#include "Modeler.h"

namespace ATK
{
  Modeler::Modeler(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_input_pins)
  :nb_dynamic_pins(nb_dynamic_pins), nb_static_pins(nb_static_pins), nb_input_pins(nb_input_pins), dynamic_pins(nb_dynamic_pins), static_pins(nb_static_pins), input_pins(nb_input_pins)
  {
  }
  
  Modeler::~Modeler()
  {
  }
}

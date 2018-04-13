/**
 * \file Component.cpp
 */

#include "Component.h"

namespace ATK
{
  Component::~Component()
  {
  }
  
  void Component::set_pins(std::vector<std::tuple<PinType, gsl::index>> pins)
  {
    this->pins = std::move(pins);
  }

  void Component::update_model(Modeler* modeler)
  {
  }
  
  void Component::update_steady_state(DataType dt)
  {
  }
  
  void Component::update_state()
  {
  }
  
  void Component::precompute(bool steady_state)
  {
  }
  
  void Component::add_equation(gsl::index eq_number, bool steady_state)
  {
  }
}

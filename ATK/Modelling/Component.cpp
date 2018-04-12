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
}

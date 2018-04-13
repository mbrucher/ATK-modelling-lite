/**
 * \file Modeler.cpp
 */

#include "Component.h"
#include "Modeler.h"

namespace ATK
{
  Modeler::Modeler(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_input_pins)
  :nb_dynamic_pins(nb_dynamic_pins), nb_static_pins(nb_static_pins), nb_input_pins(nb_input_pins), dynamic_pins(nb_dynamic_pins), static_pins(nb_static_pins), input_pins(nb_input_pins), dynamic_state(nb_dynamic_pins, 0), static_state(nb_static_pins, 0), input_state(nb_input_pins, 0)
  {
  }
  
  Modeler::~Modeler()
  {
  }
  
  std::vector<std::vector<std::tuple<Component*, gsl::index>>>& Modeler::get_pins(PinType type)
  {
    switch(type)
    {
      case PinType::Static:
        return static_pins;
      case PinType::Dynamic:
        return dynamic_pins;
      case PinType::Input:
        return input_pins;
    }
  }

  void Modeler::set_dt(DataType dt)
  {
    this->dt = dt;
  }

  void Modeler::add_component(std::unique_ptr<Component> component, std::vector<std::tuple<PinType, gsl::index>> pins)
  {
    for(gsl::index i = 0; i < pins.size(); ++i)
    {
      get_pins(std::get<0>(pins[i]))[std::get<1>(pins[i])].push_back(std::make_tuple(component.get(), i));
    }
    component->set_pins(std::move(pins));
    component->update_model(this);
    components.insert(std::move(component));
  }
  
  void Modeler::setup(bool steady_state)
  {
    for(auto& component : components)
    {
      component->update_steady_state(dt);
    }
    
    if(steady_state)
    {
      //solve(steady_state);
      
      for(auto& component : components)
      {
        component->update_steady_state(dt);
      }
    }
    initialized = true;
  }
}

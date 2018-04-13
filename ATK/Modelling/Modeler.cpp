/**
 * \file Modeler.cpp
 */

#include "Component.h"
#include "Modeler.h"

static constexpr gsl::index MAX_ITERATION = 200;

namespace ATK
{
  Modeler::Modeler(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_input_pins)
  :nb_dynamic_pins(nb_dynamic_pins)
  , nb_static_pins(nb_static_pins)
  , nb_input_pins(nb_input_pins)
  , dynamic_pins(nb_dynamic_pins)
  , dynamic_pins_equation(nb_dynamic_pins, std::make_tuple(nullptr, -1))
  , static_pins(nb_static_pins)
  , input_pins(nb_input_pins)
  , dynamic_state(nb_dynamic_pins, 0)
  , static_state(nb_static_pins, 0)
  , input_state(nb_input_pins, 0)
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
  
  void Modeler::set_static_state(std::vector<DataType> static_state)
  {
    this->static_state = std::move(static_state);
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
      solve(steady_state);
      
      for(auto& component : components)
      {
        component->update_steady_state(dt);
      }
    }
    initialized = true;
  }
  
  const std::vector<typename Modeler::DataType>& Modeler::operator()(std::vector<DataType> input_state)
  {
    if(!initialized)
    {
      setup();
    }
    this->input_state = std::move(input_state);
    
    solve(true);
      
    for(auto& component : components)
    {
      component->update_state();
    }

    return dynamic_state;
  }


  void Modeler::solve(bool steady_state)
  {
    gsl::index iteration = 0;
    
    while(iteration < MAX_ITERATION && !iterate(steady_state))
    {
      ++iteration;
    }
  }

  bool Modeler::iterate(bool steady_state)
  {
    return false;
  }
}

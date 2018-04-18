/**
 * \file ModellerFilter.cpp
 */

#include "Component.h"
#include "ModellerFilter.h"

static constexpr gsl::index MAX_ITERATION = 200;
static constexpr double EPS = 1e-8;

namespace ATK
{
  template<typename DataType_>
  ModellerFilter<DataType_>::ModellerFilter(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_input_pins)
  : TypedBaseFilter<DataType_>(nb_input_pins, nb_dynamic_pins)
  , nb_dynamic_pins(nb_dynamic_pins)
  , nb_static_pins(nb_static_pins)
  , nb_input_pins(nb_input_pins)
  , dynamic_pins(nb_dynamic_pins)
  , dynamic_pins_equation(nb_dynamic_pins, std::make_tuple(nullptr, -1))
  , static_pins(nb_static_pins)
  , input_pins(nb_input_pins)
  , dynamic_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1>::Zero(nb_dynamic_pins))
  , static_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1>::Zero(nb_static_pins))
  , input_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1>::Zero(nb_input_pins))
  {
  }
  
  template<typename DataType_>
  ModellerFilter<DataType_>::~ModellerFilter()
  {
  }
  
  template<typename DataType_>
  std::vector<std::vector<std::tuple<Component<DataType_>*, gsl::index>>>& ModellerFilter<DataType_>::get_pins(PinType type)
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

  template<typename DataType_>
  const Eigen::Matrix<typename ModellerFilter<DataType_>::DataType, Eigen::Dynamic, 1>& ModellerFilter<DataType_>::get_states(PinType type) const
  {
    switch(type)
    {
      case PinType::Static:
        return static_state;
      case PinType::Dynamic:
        return dynamic_state;
      case PinType::Input:
        return input_state;
    }
  }
  
  template<typename DataType_>
  typename ModellerFilter<DataType_>::DataType ModellerFilter<DataType_>::retrieve_voltage(const std::tuple<PinType, gsl::index>& pin) const
  {
    return get_states(std::get<0>(pin))[std::get<1>(pin)];
  }

  template<typename DataType_>
  void ModellerFilter<DataType_>::set_static_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1> static_state)
  {
    this->static_state = std::move(static_state);
  }

  template<typename DataType_>
  void ModellerFilter<DataType_>::add_component(std::unique_ptr<Component<DataType_>> component, std::vector<std::tuple<PinType, gsl::index>> pins)
  {
    for(gsl::index i = 0; i < pins.size(); ++i)
    {
      get_pins(std::get<0>(pins[i]))[std::get<1>(pins[i])].push_back(std::make_tuple(component.get(), i));
    }
    component->set_pins(std::move(pins));
    component->update_model(this);
    components.insert(std::move(component));
  }
  
  template<typename DataType_>
  void ModellerFilter<DataType_>::init(bool steady_state)
  {
    for(auto& component : components)
    {
      component->update_steady_state(1. / input_sampling_rate);
    }
    
    if(steady_state)
    {
      solve(steady_state);
      
      for(auto& component : components)
      {
        component->update_steady_state(1. / input_sampling_rate);
      }
    }
    initialized = true;
  }
  
  template<typename DataType_>
  void ModellerFilter<DataType_>::setup()
  {
    assert(input_sampling_rate == output_sampling_rate);
    
    if(!initialized)
    {
      init(true);
    }
  }

  template<typename DataType_>
  void ModellerFilter<DataType_>::process_impl(size_t size) const
  {
    for(gsl::index i = 0; i < size; ++i)
    {
      for(gsl::index j = 0; j < nb_input_ports; ++j)
      {
        input_state[j] = converted_inputs[j][i];
      }

      solve(true);

      for (auto& component : components)
      {
        component->update_state();
      }

      for(gsl::index j = 0; j < nb_output_ports; ++j)
      {
        outputs[j][i] = dynamic_state[j];
      }
    }
  }


  template<typename DataType_>
  void ModellerFilter<DataType_>::solve(bool steady_state) const
  {
    gsl::index iteration = 0;
    
    while(iteration < MAX_ITERATION && !iterate(steady_state))
    {
      ++iteration;
    }
  }

  template<typename DataType_>
  bool ModellerFilter<DataType_>::iterate(bool steady_state) const
  {
    for(auto& component : components)
    {
      component->precompute(steady_state);
    }
    
    Eigen::Matrix<DataType, Eigen::Dynamic, 1> eqs(Eigen::Matrix<DataType, Eigen::Dynamic, 1>::Zero(nb_dynamic_pins));
    Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic> jacobian(Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>::Zero(nb_dynamic_pins, nb_dynamic_pins));
    
    // Populate the equations + jacobian for computing next update
    for(gsl::index i = 0; i < nb_dynamic_pins; ++i)
    {
      if(std::get<0>(dynamic_pins_equation[i]) == nullptr)
      {
        compute_current(i, eqs, jacobian, steady_state);
      }
      else
      {
        //component, eq_number = self.dynamic_pins_equation[i]
        //eq, jac = component.add_equation(self.state, steady_state, eq_number)
      }
    }

    // Check if the equations have converged
    if((eqs.array().abs() < EPS).all())
    {
      return true;
    }

    Eigen::Matrix<DataType, Eigen::Dynamic, 1> delta = jacobian.colPivHouseholderQr().solve(eqs);

    // Check if the update is big enough
    if((delta.array().abs() < EPS).all())
    {
      return true;
    }
    
    dynamic_state -= delta;

    return false;
  }

  template<typename DataType_>
  void ModellerFilter<DataType_>::compute_current(gsl::index i, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state) const
  {
    DataType& current = eqs(i);
    for(const auto& component: dynamic_pins[i])
    {
      current += std::get<0>(component)->get_current(std::get<1>(component), steady_state);
      
      const auto& pins = std::get<0>(component)->get_pins();
      
      for(gsl::index j = 0; j < pins.size(); ++j)
      {
        if(std::get<0>(pins[j]) == PinType::Dynamic)
        {
          jacobian(i, std::get<1>(pins[j])) += std::get<0>(component)->get_gradient(std::get<1>(component), j, steady_state);
        }
      }
    }
  }
  
  template class ModellerFilter<double>;
}

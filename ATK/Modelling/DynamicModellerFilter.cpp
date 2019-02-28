/**
 * \file DynamicModellerFilter.cpp
 */

#if ENABLE_LOG
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
#endif

#include "Component.h"
#include "DynamicModellerFilter.h"

#include <ATK/Core/Utilities.h>

constexpr gsl::index MAX_ITERATION = 200;
constexpr double EPS = 1e-8;
constexpr gsl::index INIT_WARMUP = 10;
constexpr double MAX_DELTA = 1e-1;

namespace ATK
{
  template<typename DataType_>
  DynamicModellerFilter<DataType_>::DynamicModellerFilter(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_input_pins)
  : ModellerFilter<DataType_>(nb_dynamic_pins, nb_input_pins)
  , nb_dynamic_pins(nb_dynamic_pins)
  , nb_static_pins(nb_static_pins)
  , nb_input_pins(nb_input_pins)
  , dynamic_pins(nb_dynamic_pins)
  , dynamic_pins_equation(nb_dynamic_pins, std::make_tuple(nullptr, -1))
  , dynamic_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1>::Zero(nb_dynamic_pins))
  , static_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1>::Zero(nb_static_pins))
  , input_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1>::Zero(nb_input_pins))
  , initialized(false)
  {
  }
  
  template<typename DataType_>
  DynamicModellerFilter<DataType_>::~DynamicModellerFilter()
  {
  }
  
  template<typename DataType_>
  const Eigen::Matrix<typename DynamicModellerFilter<DataType_>::DataType, Eigen::Dynamic, 1>& DynamicModellerFilter<DataType_>::get_states(PinType type) const
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
  typename DynamicModellerFilter<DataType_>::DataType DynamicModellerFilter<DataType_>::retrieve_voltage(const std::tuple<PinType, gsl::index>& pin) const
  {
    return get_states(std::get<0>(pin))[std::get<1>(pin)];
  }

  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::set_static_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1> static_state)
  {
    this->static_state = std::move(static_state);
  }

  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::add_component(std::unique_ptr<Component<DataType_>> component, std::vector<std::tuple<PinType, gsl::index>> pins)
  {
    for(gsl::index i = 0; i < pins.size(); ++i)
    {
      if(std::get<0>(pins[i]) == PinType::Dynamic)
      {
          dynamic_pins[std::get<1>(pins[i])].push_back(std::make_tuple(component.get(), i));
      }
    }
    component->set_pins(std::move(pins));
    component->update_model(this);
    components.insert(std::move(component));
  }
  
  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::set_custom_equation(gsl::index eq, std::tuple<Component<DataType>*, gsl::index> custom_equation)
  {
    dynamic_pins_equation[eq] = custom_equation;
  }

  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::init()
  {
    for(auto& component : components)
    {
      component->update_steady_state(1. / input_sampling_rate);
    }
    
    solve(true);
      
    for(auto& component : components)
    {
      component->update_steady_state(1. / input_sampling_rate);
    }
#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "init state: " << dynamic_state;
#endif
    
    initialized = true;
  }
  
  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::setup()
  {
    assert(input_sampling_rate == output_sampling_rate);
    
    if(!initialized)
    {
      auto target_static_state = static_state;
      
      for(gsl::index i = 0; i < INIT_WARMUP; ++i)
      {
        static_state = target_static_state * ((i+1.) / INIT_WARMUP);
        init();
      }
      static_state = target_static_state;
    }
  }

  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::process_impl(gsl::index size) const
  {
    for(gsl::index i = 0; i < size; ++i)
    {
      for(gsl::index j = 0; j < nb_input_ports; ++j)
      {
        input_state[j] = converted_inputs[j][i];
      }

      solve(false);
#if ENABLE_LOG
      BOOST_LOG_TRIVIAL(trace) << "final state: " << dynamic_state;
#endif
      
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
  void DynamicModellerFilter<DataType_>::solve(bool steady_state) const
  {
    gsl::index iteration = 0;
    
    while(iteration < MAX_ITERATION && !iterate(steady_state))
    {
      ++iteration;
    }
#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "total iterations: " << iteration;
#endif
  }

  template<typename DataType_>
  bool DynamicModellerFilter<DataType_>::iterate(bool steady_state) const
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
        std::get<0>(dynamic_pins_equation[i])->add_equation(i, std::get<1>(dynamic_pins_equation[i]), eqs, jacobian, steady_state);
      }
    }

#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "eqs: " << eqs;
    BOOST_LOG_TRIVIAL(trace) << "jacobian: " << jacobian;
#endif
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
    
    auto max_delta = delta.array().abs().maxCoeff();
    if(max_delta > MAX_DELTA)
    {
      delta *= MAX_DELTA / max_delta;
    }
    
    dynamic_state -= delta;
#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "delta: " << delta;
    BOOST_LOG_TRIVIAL(trace) << "intermediate state: " << dynamic_state;
#endif
    
    return false;
  }

  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::compute_current(gsl::index i, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state) const
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

  template<typename DataType_>
  gsl::index DynamicModellerFilter<DataType_>::get_number_parameters() const
  {
    return std::accumulate(begin(components), end(components), 0, [](gsl::index i, const auto& component)
                           {
                             return i + component->get_number_parameters();
                           }
           );
  }
  
  template<typename Components, typename Function>
  auto scan_components(const Components& components, gsl::index identifier, Function fun)
  {
    gsl::index ind = 0;
    
    for(const auto& component: components)
    {
      if(component->get_number_parameters() == 0)
      {
        continue;
      }
      if(ind + component->get_number_parameters() >= identifier)
      {
        ind += component->get_number_parameters();
        continue;
      }
      return fun(component, identifier - ind);
    }
    throw RuntimeError("No such parameter");
  }
  
  template<typename DataType_>
  std::string DynamicModellerFilter<DataType_>::get_parameter_name(gsl::index identifier) const
  {
    return scan_components(components, identifier, [&](const auto& component, gsl::index i){
      return component->get_parameter_name(i);
    });
  }
  
  template<typename DataType_>
  DataType_ DynamicModellerFilter<DataType_>::get_parameter(gsl::index identifier) const
  {
    return scan_components(components, identifier, [&](const auto& component, gsl::index i){
      return component->get_parameter(i);
    });
  }
  
  template<typename DataType_>
  void DynamicModellerFilter<DataType_>::set_parameter(gsl::index identifier, DataType_ value)
  {
    return scan_components(components, identifier, [&](const auto& component, gsl::index i){
      component->set_parameter(i, value);
    });
    throw RuntimeError("No such parameter");
  }

  template class DynamicModellerFilter<double>;
}

/**
 * \file SPICEHandler.cpp
 */

#include <memory>

#if ENABLE_LOG
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
#endif

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/Capacitor.h>
#include <ATK/Modelling/Coil.h>
#include <ATK/Modelling/Current.h>
#include <ATK/Modelling/Diode.h>
#include <ATK/Modelling/DynamicModellerFilter.h>
#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/Resistor.h>
#include <ATK/Modelling/Transistor.h>
#include <ATK/Modelling/VoltageGain.h>
#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/SPICE/parser.h>
#include <ATK/Modelling/SPICE/Utilities.h>

namespace ATK
{
  template<typename DataType>
  SPICEHandler<DataType>::SPICEHandler(const ast::SPICEAST& tree)
  :tree(tree)
  {
  }

  template<typename DataType>
  std::tuple<gsl::index, gsl::index, gsl::index> SPICEHandler<DataType>::get_pins() const
  {
    return std::make_tuple(static_pins.size(), input_pins.size(), dynamic_pins.size());
  }
  
  template<typename DataType>
  std::unique_ptr<ModellerFilter<DataType>> SPICEHandler<DataType>::convert(const ast::SPICEAST& tree)
  {
    SPICEHandler<DataType> handler(tree);
    handler.process();
    
    auto [nb_static_pins, nb_input_pins, nb_dynamic_pins] = handler.get_pins();
    
#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "Static pins: " << nb_static_pins;
    BOOST_LOG_TRIVIAL(trace) << "Input pins: " << nb_input_pins;
    BOOST_LOG_TRIVIAL(trace) << "Dynamic pins: " << nb_dynamic_pins;
#endif

    auto filter = std::make_unique<DynamicModellerFilter<DataType>>(nb_dynamic_pins, nb_static_pins, nb_input_pins);
    filter->set_static_state(handler.get_static_state());
    
    for(auto& component: handler.components)
    {
      filter->add_component(std::move(std::get<0>(component)), std::move(std::get<1>(component)));
    }
    
    return std::unique_ptr<ModellerFilter<DataType>>(std::move(filter));
  }
  
  template<typename DataType>
  void SPICEHandler<DataType>::process()
  {
    set_static_pins(tree, static_pins, static_voltage, input_pins, pins);
    generate_components();
  }

  template<typename DataType>
  std::unique_ptr<Component<DataType>> SPICEHandler<DataType>::create_component(const std::string& model_name) const
  {
    auto model = tree.models.find(model_name);
    if(model == tree.models.end())
    {
      throw RuntimeError("Unknown model named " + model_name);
    }
    
    if(model->second.first == "d")
    {
      DiodeHelper<DataType> helper;
      helper.populate(model->second.second);
      return std::make_unique<Diode<DataType>>(helper.is, helper.n, helper.vt);
    }
    
    if(model->second.first == "npn")
    {
      NPNHelper<DataType> helper;
      helper.populate(model->second.second);
      return std::make_unique<NPN<DataType>>(helper.is, helper.vt, helper.ne, helper.br, helper.bf);
    }
    
    if(model->second.first == "pnp")
    {
      PNPHelper<DataType> helper;
      helper.populate(model->second.second);
      return std::make_unique<PNP<DataType>>(helper.is, helper.vt, helper.ne, helper.br, helper.bf);
    }

    throw RuntimeError("Unknown model class named " + model->second.first);
  }
  
  template<typename DataType>
  void SPICEHandler<DataType>::add_capacitor(const ast::Component& component)
  {
    if(component.second.size() != 3)
    {
      throw RuntimeError("Wrong number of arguments for component " + component.first);
    }
    std::string pin0 = to_name(component.second[0]);
    add_dynamic_pin(dynamic_pins, pin0);
    std::string pin1 = to_name(component.second[1]);
    add_dynamic_pin(dynamic_pins, pin1);
    double value = convert_component_value(boost::get<ast::SPICENumber>(component.second[2]));
    components.push_back(std::make_tuple(std::make_unique<Capacitor<DataType>>(value), std::vector<Pin>{pins[pin0], pins[pin1]}));

#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "Adding capacitor: " << value << "\t" << pin0 << "\t" << pin1;
#endif
  }

  template<typename DataType>
  void SPICEHandler<DataType>::add_coil(const ast::Component& component)
  {
    if(component.second.size() != 3)
    {
      throw RuntimeError("Wrong number of arguments for component " + component.first);
    }
    std::string pin0 = to_name(component.second[0]);
    add_dynamic_pin(dynamic_pins, pin0);
    std::string pin1 = to_name(component.second[1]);
    add_dynamic_pin(dynamic_pins, pin1);
    double value = convert_component_value(boost::get<ast::SPICENumber>(component.second[2]));
    components.push_back(std::make_tuple(std::make_unique<Coil<DataType>>(value), std::vector<Pin>{pins[pin0], pins[pin1]}));
    
#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "Adding coil: " << value << "\t" << pin0 << "\t" << pin1;
#endif
  }

  template<typename DataType>
  void SPICEHandler<DataType>::add_diode(const ast::Component& component)
  {
    if(component.second.size() != 3)
    {
      throw RuntimeError("Wrong number of arguments for component " + component.first);
    }
    std::string pin0 = to_name(component.second[0]);
    add_dynamic_pin(dynamic_pins, pin0);
    std::string pin1 = to_name(component.second[1]);
    add_dynamic_pin(dynamic_pins, pin1);
    std::string diode_model = to_name(component.second[2]);
    components.push_back(std::make_tuple(create_component(diode_model), std::vector<Pin>{pins[pin0], pins[pin1]}));
  }

  template<typename DataType>
  void SPICEHandler<DataType>::add_resistance(const ast::Component& component)
  {
    if(component.second.size() != 3)
    {
      throw RuntimeError("Wrong number of arguments for component " + component.first);
    }
    std::string pin0 = to_name(component.second[0]);
    add_dynamic_pin(dynamic_pins, pin0);
    std::string pin1 = to_name(component.second[1]);
    add_dynamic_pin(dynamic_pins, pin1);
    double value = convert_component_value(boost::get<ast::SPICENumber>(component.second[2]));
    components.push_back(std::make_tuple(std::make_unique<Resistor<DataType>>(value), std::vector<Pin>{pins[pin0], pins[pin1]}));

#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "Adding resistor: " << value << "\t" << pin0 << "\t" << pin1;
#endif
  }

  template<typename DataType>
  void SPICEHandler<DataType>::add_transistor(const ast::Component& component)
  {
    if(component.second.size() != 4)
    {
      throw RuntimeError("Wrong number of arguments for component " + component.first);
    }
    std::string pin0 = to_name(component.second[0]);
    add_dynamic_pin(dynamic_pins, pin0);
    std::string pin1 = to_name(component.second[1]);
    add_dynamic_pin(dynamic_pins, pin1);
    std::string pin2 = to_name(component.second[2]);
    add_dynamic_pin(dynamic_pins, pin2);
    std::string transistor_model = to_name(component.second[3]);
    components.push_back(std::make_tuple(create_component(transistor_model), std::vector<Pin>{pins[pin1], pins[pin0], pins[pin2]}));
  }

  template<typename DataType>
  void SPICEHandler<DataType>::add_current(const ast::Component& component)
  {
    if(component.second.size() != 3)
    {
      throw RuntimeError("Wrong number of arguments for component " + component.first);
    }
    std::string pin0 = to_name(component.second[0]);
    add_dynamic_pin(dynamic_pins, pin0);
    std::string pin1 = to_name(component.second[1]);
    add_dynamic_pin(dynamic_pins, pin1);
    double value = convert_component_value(boost::get<ast::SPICENumber>(component.second[2]));
    components.push_back(std::make_tuple(std::make_unique<Current<DataType>>(value), std::vector<Pin>{pins[pin0], pins[pin1]}));
    
#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "Adding current: " << value << "\t" << pin0 << "\t" << pin1;
#endif
  }

  template<typename DataType>
  void SPICEHandler<DataType>::add_voltage_multiplier(const ast::Component& component)
  {
    if(component.second.size() != 5)
    {
      throw RuntimeError("Wrong number of arguments for component " + component.first);
    }
    std::string pin0 = to_name(component.second[0]);
    add_dynamic_pin(dynamic_pins, pin0);
    std::string pin1 = to_name(component.second[1]);
    add_dynamic_pin(dynamic_pins, pin1);
    std::string pin2 = to_name(component.second[2]);
    add_dynamic_pin(dynamic_pins, pin2);
    std::string pin3 = to_name(component.second[3]);
    add_dynamic_pin(dynamic_pins, pin3);
    double value = convert_component_value(boost::get<ast::SPICENumber>(component.second[4]));
    components.push_back(std::make_tuple(std::make_unique<VoltageGain<DataType>>(value), std::vector<Pin>{pins[pin2], pins[pin3], pins[pin0], pins[pin1]}));

#if ENABLE_LOG
    BOOST_LOG_TRIVIAL(trace) << "Adding voltage multiplier: " << value << "\t" << pin2 << "\t" << pin3 << "\t" << pin0 << "\t" << pin1;
#endif
  }
  
  template<typename DataType>
  void SPICEHandler<DataType>::generate_components()
  {
    for(const auto& component: tree.components)
    {
#if ENABLE_LOG
      BOOST_LOG_TRIVIAL(trace) << "Adding component: " << component.first;
#endif
      switch(component.first[0])
      {
        case 'c':
        {
          add_capacitor(component);
          break;
        }
        case 'd':
        {
          add_diode(component);
          break;
        }
        case 'e':
        {
          add_voltage_multiplier(component);
          break;
        }
        case 'i':
        {
          add_current(component);
          break;
        }
        case 'l':
        {
          add_coil(component);
          break;
        }
        case 'q':
        {
          add_transistor(component);
          break;
        }
        case 'r':
        {
          add_resistance(component);
          break;
        }
        case 'v':
          break;
        default:
          throw RuntimeError("Unknown component for name " + component.first);
      }
    }
  }
  
  template<typename DataType>
  void SPICEHandler<DataType>::add_dynamic_pin(std::unordered_set<std::string>& map, const std::string& pin)
  {
    if(pins.find(pin) == pins.end())
    {
#if ENABLE_LOG
      BOOST_LOG_TRIVIAL(trace) << "Adding dynamic pin " << pin << " index " << map.size();
#endif
      add_pin(map, PinType::Dynamic, pin, pins);
    }
  }

  template<typename DataType>
  Eigen::Matrix<DataType, Eigen::Dynamic, 1> SPICEHandler<DataType>::get_static_state() const
  {
    Eigen::Matrix<DataType, Eigen::Dynamic, 1> state = Eigen::Matrix<DataType, Eigen::Dynamic, 1>(static_voltage.size());
    for(size_t i = 0; i < static_voltage.size(); ++i)
    {
      state(i) = static_voltage[i];
    }
    return state;
  }

  template<typename DataType>
  auto SPICEHandler<DataType>::get_components() const -> const decltype(components)&
  {
    return components;
  }

  template class SPICEHandler<double>;
}

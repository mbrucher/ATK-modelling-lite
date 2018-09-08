/**
 * \file SPICEHandler.cpp
 */

#include <memory>

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/Capacitor.h>
#include <ATK/Modelling/Coil.h>
#include <ATK/Modelling/Diode.h>
#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/Resistor.h>
#include <ATK/Modelling/Transistor.h>
#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/SPICE/parser.h>

namespace
{
  class NameVisitor : public boost::static_visitor<std::string>
  {
  public:
    std::string operator()(const ATK::ast::SPICENumber& number) const
    {
      return std::to_string(int(number.first));
    }
    
    std::string operator()(const std::string & str) const
    {
      return str;
    }
  };

  std::string to_name(const ATK::ast::SPICEArg& arg)
  {
    return boost::apply_visitor(NameVisitor(), arg);
  }
}

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
    
    auto filter = std::make_unique<ModellerFilter<DataType>>(nb_dynamic_pins, nb_static_pins, nb_input_pins);
    filter->set_static_state(handler.get_static_state());
    
    for(auto& component: handler.components)
    {
      filter->add_component(std::move(component.first), std::move(component.second));
    }
    
    return std::move(filter);
  }
  
  template<typename DataType>
  void SPICEHandler<DataType>::process()
  {
    set_static_pins();
    generate_components();
  }

  template<typename DataType>
  void SPICEHandler<DataType>::set_static_pins()
  {
    static_pins.insert("0");
    pins.insert(std::make_pair("gnd", std::make_pair(PinType::Static, 0)));
    pins.insert(std::make_pair("0", std::make_pair(PinType::Static, 0)));
    static_voltage.push_back(0);
    
    for(const auto& component: tree.components)
    {
      if(component.first[0] == 'v')
      {
        if(component.second.size() < 3)
        {
          throw ATK::RuntimeError("Voltage " + component.first + " is missing values, only " + std::to_string(component.second.size() ));
        }

        std::string pin0 = to_name(component.second[0]);
        std::string pin1 = to_name(component.second[1]);
        auto first_gnd = (pin0 == "0" || pin0 == "gnd");
        
        if(pins.find(pin0) == pins.end() && pins.find(pin1) == pins.end())
        {
          throw ATK::RuntimeError("At least one voltage needs to be set to ground");
        }
        if(component.second.size() == 3)
        {
          add_dual_pin(static_pins, PinType::Static, pin0, pin1, first_gnd);
          static_voltage.push_back((first_gnd ? -1 : 1) * convert_component_value(boost::get<ast::SPICENumber>(component.second[2])));
        }
        else if(component.second.size() == 4)
        {
          if(boost::get<std::string>(component.second[2]) == "DC")
          {
            add_dual_pin(static_pins, PinType::Static, pin0, pin1, first_gnd);
            static_voltage.push_back((first_gnd ? -1 : 1) * convert_component_value(boost::get<ast::SPICENumber>(component.second[3])));
          }
          else
          {
            add_dual_pin(input_pins, PinType::Input, pin0, pin1, first_gnd);
          }
        }
        else if(component.second.size() >= 5)
        {
          add_dual_pin(input_pins, PinType::Input, pin0, pin1, first_gnd);
        }
      }
    }
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
      return std::make_unique<Diode<DataType>>();
    }
    
    if(model->second.first == "npn")
    {
      return std::make_unique<NPN<DataType>>();
    }
    
    if(model->second.first == "pnp")
    {
      return std::make_unique<PNP<DataType>>();
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
    components.push_back(std::make_pair(std::make_unique<Capacitor<DataType>>(value), std::vector<Pin>{pins[pin0], pins[pin1]}));
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
    components.push_back(std::make_pair(std::make_unique<Coil<DataType>>(value), std::vector<Pin>{pins[pin0], pins[pin1]}));
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
    components.push_back(std::make_pair(create_component(diode_model), std::vector<Pin>{pins[pin0], pins[pin1]}));
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
    components.push_back(std::make_pair(std::make_unique<Resistor<DataType>>(value), std::vector<Pin>{pins[pin0], pins[pin1]}));
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
    components.push_back(std::make_pair(create_component(transistor_model), std::vector<Pin>{pins[pin1], pins[pin0], pins[pin2]}));
  }

  template<typename DataType>
  void SPICEHandler<DataType>::generate_components()
  {
    for(const auto& component: tree.components)
    {
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
  void SPICEHandler<DataType>::add_dual_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin0, const std::string& pin1, bool first_gnd)
  {
    if(first_gnd)
    {
      add_pin(map, type, pin1);
    }
    else
    {
      add_pin(map, type, pin0);
    }
  }
  
  template<typename DataType>
  void SPICEHandler<DataType>::add_dynamic_pin(std::unordered_set<std::string>& map, const std::string& pin)
  {
    if(pins.find(pin) == pins.end())
    {
      add_pin(map, PinType::Dynamic, pin);
    }
  }
  
  template<typename DataType>
  void SPICEHandler<DataType>::add_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin)
  {
    pins.insert(std::make_pair(pin, std::make_tuple(type, map.size())));
    map.insert(pin);
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

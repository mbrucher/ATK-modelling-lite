/**
 * \file SPICEHandler.cpp
 */

#include <memory>

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/SPICE/parser.h>

namespace
{
  class PinVisitor : public boost::static_visitor<std::string>
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

  std::string to_pin(const ATK::ast::SPICEArg& arg)
  {
    return boost::apply_visitor(PinVisitor(), arg);
  }
}

namespace ATK
{
  SPICEHandler::SPICEHandler(const ast::SPICEAST& tree)
  :tree(tree)
  {
  }

  std::tuple<gsl::index, gsl::index, gsl::index> SPICEHandler::get_pins() const
  {
    return std::make_tuple(static_pins.size(), input_pins.size(), dynamic_pins.size());
  }
  
  template<typename DataType>
  std::unique_ptr<ModellerFilter<DataType>> SPICEHandler::convert(const ast::SPICEAST& tree)
  {
    SPICEHandler handler(tree);
    handler.process();
    
    auto [nb_static_pins, nb_input_pins, nb_dynamic_pins] = handler.get_pins();
    
    auto filter = std::make_unique<ModellerFilter<DataType>>(nb_dynamic_pins, nb_static_pins, nb_input_pins);
    
    return std::move(filter);
  }
  
  void SPICEHandler::process()
  {
    set_static_pins();
  }

  void SPICEHandler::set_static_pins()
  {
    static_pins.insert("0");
    pins.insert(std::make_pair("gnd", std::make_pair(PinType::Static, 0)));
    pins.insert(std::make_pair("0", std::make_pair(PinType::Static, 0)));
    
    for(const auto& component: tree.components)
    {
      if(component.first[0] == 'v')
      {
        if(component.second.size() < 3)
        {
          throw ATK::RuntimeError("Voltage " + component.first + " is missing values, only " + std::to_string(component.second.size() ));
        }

        std::string pin0 = to_pin(component.second[0]);
        std::string pin1 = to_pin(component.second[1]);
        auto first_gnd = (pin0 == "0" || pin0 == "gnd");
        
        if(pins.find(pin0) == pins.end() && pins.find(pin1) == pins.end())
        {
          throw ATK::RuntimeError("At least one voltage needs to be set to ground");
        }
        if(component.second.size() == 3)
        {
          add_pin(static_pins, PinType::Static, pin0, pin1, first_gnd);
        }
        else if(component.second.size() == 4)
        {
          if(boost::get<std::string>(component.second[2]) == "DC")
          {
            add_pin(static_pins, PinType::Static, pin0, pin1, first_gnd);
          }
          else
          {
            add_pin(input_pins, PinType::Input, pin0, pin1, first_gnd);
          }
        }
        else if(component.second.size() >= 5)
        {
          add_pin(input_pins, PinType::Input, pin0, pin1, first_gnd);
        }
      }
    }
  }

  void SPICEHandler::add_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin0, const std::string& pin1, bool first_gnd)
  {
    if(first_gnd)
    {
      pins.insert(std::make_pair(pin1, std::make_pair(type, map.size())));
      map.insert(pin1);
    }
    else
    {
      pins.insert(std::make_pair(pin0, std::make_pair(type, map.size())));
      map.insert(pin0);
    }
  }

  template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> SPICEHandler::convert<double>(const ast::SPICEAST& tree);

}

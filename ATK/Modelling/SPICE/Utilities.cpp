/**
 * \file Utilities.cpp
 */

#include <memory>

#if ENABLE_LOG
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
#endif

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/SPICE/Utilities.h>

namespace ATK
{
  void set_static_pins(const ast::SPICEAST& tree, std::unordered_set<std::string>& static_pins, std::vector<double>& static_voltage, std::unordered_set<std::string>& input_pins, std::unordered_map<std::string, Pin>& pins)
  {
    static_pins.insert("0");
    pins.insert(std::make_pair("gnd", std::make_tuple(PinType::Static, 0)));
    pins.insert(std::make_pair("0", std::make_tuple(PinType::Static, 0)));
    static_voltage.push_back(0);
    
    for(const auto& component: tree.components)
    {
      if(component.first[0] == 'v')
      {
#if ENABLE_LOG
        BOOST_LOG_TRIVIAL(trace) << "Adding voltage: " << component.first;
#endif
        if(component.second.size() < 3)
        {
          throw ATK::RuntimeError("Voltage " + component.first + " is missing values, only " + std::to_string(component.second.size()));
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
          add_dual_pin(static_pins, PinType::Static, pin0, pin1, first_gnd, pins);
          static_voltage.push_back((first_gnd ? -1 : 1) * convert_component_value(boost::get<ast::SPICENumber>(component.second[2])));
        }
        else if(component.second.size() == 4)
        {
          if(boost::get<std::string>(component.second[2]) == "dc")
          {
            add_dual_pin(static_pins, PinType::Static, pin0, pin1, first_gnd, pins);
            static_voltage.push_back((first_gnd ? -1 : 1) * convert_component_value(boost::get<ast::SPICENumber>(component.second[3])));
          }
          else
          {
            add_dual_pin(input_pins, PinType::Input, pin0, pin1, first_gnd, pins);
          }
        }
        else if(component.second.size() >= 5)
        {
          add_dual_pin(input_pins, PinType::Input, pin0, pin1, first_gnd, pins);
        }
      }
    }
  }

  void add_dual_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin0, const std::string& pin1, bool first_gnd, std::unordered_map<std::string, Pin>& pins)
  {
    if(first_gnd)
    {
      add_pin(map, type, pin1, pins);
    }
    else
    {
      add_pin(map, type, pin0, pins);
    }
  }
  
  void add_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin, std::unordered_map<std::string, Pin>& pins)
  {
    pins.insert(std::make_pair(pin, std::make_tuple(type, map.size())));
    map.insert(pin);
  }
}

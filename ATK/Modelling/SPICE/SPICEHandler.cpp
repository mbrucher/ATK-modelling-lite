/**
 * \file SPICEHandler.cpp
 */

#include <memory>

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/SPICE/parser.h>

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
    static_pins.insert("0");
    pins.insert(std::make_pair("gnd", std::make_pair(PinType::Static, 0)));
    pins.insert(std::make_pair("0", std::make_pair(PinType::Static, 0)));
  }

  template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> SPICEHandler::convert<double>(const ast::SPICEAST& tree);

}

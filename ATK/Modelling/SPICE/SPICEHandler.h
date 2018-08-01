/**
 * \file SPICEHandler.h
 * Contains the SPICE parser for ATK Modeler
 */

#ifndef ATK_MODELLING_SPICE_SPICEHANDLER
#define ATK_MODELLING_SPICE_SPICEHANDLER

#include <ATK/Modelling/config.h>
#include <ATK/Modelling/SPICE/parser.h>

namespace ATK
{
class SPICEHandler
{
  const ast::SPICEAST& tree;
public:
  SPICEHandler(const ast::SPICEAST& tree);
  
  std::tuple<gsl::index, gsl::index, gsl::index> get_pins() const;
  
  template<typename DataType>
  static std::unique_ptr<ModellerFilter<DataType>> convert(const ast::SPICEAST& tree);

};
}

#endif


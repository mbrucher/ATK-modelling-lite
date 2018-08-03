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
  /// The AST tree on which we will work
  const ast::SPICEAST& tree;

  /// Returns the number of each type of pins
  std::tuple<gsl::index, gsl::index, gsl::index> get_pins() const;
public:
  /**
   * Constructor
   */
  SPICEHandler(const ast::SPICEAST& tree);

  // Automatic dynamic filter builder
  template<typename DataType>
  static std::unique_ptr<ModellerFilter<DataType>> convert(const ast::SPICEAST& tree);

  /// Gets through the AST tree and gets data from it
  void process();
};
}

#endif


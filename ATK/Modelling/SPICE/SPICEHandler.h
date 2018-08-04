/**
 * \file SPICEHandler.h
 * Contains the SPICE parser for ATK Modeler
 */

#ifndef ATK_MODELLING_SPICE_SPICEHANDLER
#define ATK_MODELLING_SPICE_SPICEHANDLER

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <gsl/gsl>

#include <ATK/Modelling/config.h>
#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/Types.h>
#include <ATK/Modelling/SPICE/parser.h>

namespace ATK
{
class SPICEHandler
{
  /// The AST tree on which we will work
  const ast::SPICEAST& tree;

  /// Pins name map to actual pin type and id
  std::unordered_map<std::string, std::pair<PinType, gsl::index>> pins;
  /// Set of static pins (i.e. fixed voltage)
  std::unordered_set<std::string> static_pins;
  /// Set of static pins (i.e. voltage imposed as filer inputs)
  std::unordered_set<std::string> input_pins;
  /// Set of dynamic pins (i.e. voltage to be computed)
  std::unordered_set<std::string> dynamic_pins;

  /// Gets through the AST tree and gets input pins and static voltages
  void set_static_pins();
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
  /// Returns the number of each type of pins (static, input, dynamic)
  std::tuple<gsl::index, gsl::index, gsl::index> get_pins() const;
};
}

#endif


/**
 * \file SPICEHandler.h
 * Contains the SPICE parser for ATK Modeler
 */

#ifndef ATK_MODELLING_SPICE_SPICEHANDLER
#define ATK_MODELLING_SPICE_SPICEHANDLER

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <gsl/gsl>

#include <Eigen/Eigen>

#include <ATK/Modelling/config.h>
#include <ATK/Modelling/Component.h>
#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/Types.h>
#include <ATK/Modelling/SPICE/parser.h>
#include <ATK/Modelling/SPICE/Utilities.h>

namespace ATK
{
template<typename DataType>
class ATK_MODELLING_EXPORT SPICEHandler
{
  /// The AST tree on which we will work
  const ast::SPICEAST& tree;
  
  /// Pins name map to actual pin type and id
  std::unordered_map<std::string, Pin> pins;
  /// Set of static pins (i.e. fixed voltage)
  std::unordered_set<std::string> static_pins;
  /// Set of static pins (i.e. voltage imposed as filer inputs)
  std::unordered_set<std::string> input_pins;
  /// Set of dynamic pins (i.e. voltage to be computed)
  std::unordered_set<std::string> dynamic_pins;
  /// Set of all the components
  std::list<std::tuple<std::unique_ptr<Component<DataType>>, std::vector<Pin>>> components;

  /// Static voltages given by SPICE
  std::vector<double> static_voltage;

  /// Going through all the components and populate the component set
  void generate_components();
  /// Add a dynamic pin if required
  void add_dynamic_pin(std::unordered_set<std::string>& map, const std::string& pin);

  std::unique_ptr<Component<DataType>> create_component(const std::string& model_name) const;
  
  /// Adds a capacitor to the model
  void add_capacitor(const ast::Component& component);
  /// Adds a coil to the model
  void add_coil(const ast::Component& component);
  /// Adds a diode to the model
  void add_diode(const ast::Component& component);
  /// Adds a resistance to the model
  void add_resistance(const ast::Component& component);
  /// Adds a transistor to the model
  void add_transistor(const ast::Component& component);

  /// Adds a current generator to the model
  void add_current(const ast::Component& component);
  /// Adds a voltage multiplier to the model
  void add_voltage_multiplier(const ast::Component& component);

public:
  /**
   * Constructor
   */
  SPICEHandler(const ast::SPICEAST& tree);

  SPICEHandler(const SPICEHandler&) = delete;

  // Automatic dynamic filter builder
  static std::unique_ptr<ModellerFilter<DataType>> convert(const ast::SPICEAST& tree);

  /// Gets through the AST tree and gets data from it
  void process();
  /// Returns the number of each type of pins (static, input, dynamic)
  std::tuple<gsl::index, gsl::index, gsl::index> get_pins() const;
  /// Returns the known static state of the circuit
  Eigen::Matrix<DataType, Eigen::Dynamic, 1> get_static_state() const;
  /// Returns the component container
  auto get_components() const -> const decltype(components)&;
};
}

#endif


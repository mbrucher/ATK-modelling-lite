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

namespace ATK
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
  
  inline std::string to_name(const ATK::ast::SPICEArg& arg)
  {
    return boost::apply_visitor(NameVisitor(), arg);
  }

template<typename DataType>
class ATK_MODELLING_EXPORT SPICEHandler
{
  /// The AST tree on which we will work
  const ast::SPICEAST& tree;

  using Pin = std::tuple<PinType, gsl::index>;
  
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
  /// Add a new pin, with a flag
  static void add_dual_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin0, const std::string& pin1, bool first_gnd, std::unordered_map<std::string, Pin>& pins);

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
  /// Add a new pin
  static void add_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin, std::unordered_map<std::string, Pin>& pins);
  /// Gets through the AST tree and gets input pins and static voltages
  static void set_static_pins(const ast::SPICEAST& tree, std::unordered_set<std::string>& static_pins, std::vector<double>& static_voltage, std::unordered_set<std::string>& input_pins, std::unordered_map<std::string, Pin>& pins);

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


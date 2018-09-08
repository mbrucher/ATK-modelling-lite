/**
 * \file parser.h
 * Contains the SPICE parser for ATK Modeler
 */

#ifndef ATK_MODELLING_SPICE_PARSER
#define ATK_MODELLING_SPICE_PARSER

#include <ATK/Modelling/config.h>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/fusion/include/adapted.hpp>
#include <boost/fusion/include/std_pair.hpp>

namespace ATK
{
namespace ast
{
  namespace x3 = boost::spirit::x3;

  /// A SPICE number is actually a number with possibly a string after indicating its scale, or garbage (F, ohm...)
  typedef std::pair<double, std::string> SPICENumber;
  
  /// An entry on a SPICE line can be a string (pin) or a number (component value or a number-only pin)
  typedef x3::variant<std::string, SPICENumber> SPICEArg;
  
  /// Map for a component description, basically its name and then pins and values
  typedef std::unordered_map<std::string, std::vector<SPICEArg>> Components;
  /// Entry in the previous map
  typedef std::pair<std::string, std::vector<SPICEArg>> Component;
  
  /// Vector describing a model, name of the model, type and then all
  typedef std::vector<std::pair<std::string, SPICENumber>> ModelArguments;
  typedef std::pair<std::string, ModelArguments> ModelImp;
  typedef std::tuple<std::string, std::string, std::string, ModelArguments> Model;

  /// Map with all models
  typedef std::unordered_map<std::string, ModelImp> Models;

  /// End leaf of the AST, will be transformed on the fly to populate SPICEAST
  typedef x3::variant<Component, Model> SPICEEntry;

  /// The full SPICE AST
  struct SPICEAST
  {
    /// Map of existign components in the circuit
    Components components;
    /// Map of all known models
    Models models;
  };
}

/// parse one string and populate the AST accordingly
ATK_MODELLING_EXPORT void parse_string(ast::SPICEAST& ast, std::string_view str);

/// Helper function, converts an AST component value to a double
ATK_MODELLING_EXPORT double convert_component_value(const ast::SPICENumber& str);
/// Helper function for tests, parses a component value
ATK_MODELLING_EXPORT double parse_component_value(std::string_view str);
}

#endif


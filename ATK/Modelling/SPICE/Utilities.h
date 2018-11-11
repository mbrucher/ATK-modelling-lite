/**
 * \file Utilities
 * Contains the utilities  for ATK Modeler
 */

#ifndef ATK_MODELLING_SPICE_UTILITIES
#define ATK_MODELLING_SPICE_UTILITIES

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple.hpp>

#include <gsl/gsl>

#include <ATK/Modelling/config.h>
#include <ATK/Modelling/Types.h>
#include <ATK/Modelling/SPICE/parser.h>

namespace ATK
{
  using Pin = std::tuple<PinType, gsl::index>;

#define DEFINE_VARIABLE_HELPER(r, data, TUPLE) DataType BOOST_PP_TUPLE_ELEM(0, TUPLE) = BOOST_PP_TUPLE_ELEM(1, TUPLE);
#define POPULATE_VARIABLE_HELPER(r, data, TUPLE) \
if(const auto arg = args.find(BOOST_STRINGIZE(BOOST_PP_TUPLE_ELEM(0, TUPLE))); arg != args.end()) \
{ \
BOOST_PP_TUPLE_ELEM(0, TUPLE) = convert_component_value(arg->second); \
} \

#define HELPER(name, SEQ) \
template<typename DataType> \
class name \
{ \
public: \
void populate(const ast::ModelArguments& args) \
{ \
BOOST_PP_SEQ_FOR_EACH(POPULATE_VARIABLE_HELPER, _, SEQ) \
} \
\
BOOST_PP_SEQ_FOR_EACH(DEFINE_VARIABLE_HELPER, _, SEQ) \
};
  
#define DIODE_SEQ ((vt,26e-3))((is,1e-14))((n,1.24))
  HELPER(DiodeHelper, DIODE_SEQ)
#define NPN_SEQ ((vt,26e-3))((is,1e-12))((ne,1))((br,1))((bf,100))
  HELPER(NPNHelper, NPN_SEQ)
#define PNP_SEQ ((vt,26e-3))((is,1e-12))((ne,1))((br,1))((bf,100))
  HELPER(PNPHelper, PNP_SEQ)
  
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

  /// Add a new pin, with a flag
  void add_dual_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin0, const std::string& pin1, bool first_gnd, std::unordered_map<std::string, Pin>& pins);
  /// Add a new pin
  void add_pin(std::unordered_set<std::string>& map, PinType type, const std::string& pin, std::unordered_map<std::string, Pin>& pins);
  /// Gets through the AST tree and gets input pins and static voltages
  void set_static_pins(const ast::SPICEAST& tree, std::unordered_set<std::string>& static_pins, std::vector<double>& static_voltage, std::unordered_set<std::string>& input_pins, std::unordered_map<std::string, Pin>& pins);
}

#endif


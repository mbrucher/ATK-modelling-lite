/**
 * \file SPICE.cpp
 */

#include <fstream>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/io.hpp>

#include "ModellerFilter.h"
#include "SPICE.h"

namespace
{
  double transform(const std::pair<double, std::string>& value)
  {
    return value.first;
  }
}

BOOST_FUSION_ADAPT_STRUCT(ATK::AST,
                          components,
                          models
                          )

namespace ATK
{
namespace parser
{
namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;
  
using x3::lit;
using x3::lexeme;
  
using ascii::char_;
using ascii::string;

//const auto name = x3::alpha >> *x3::alnum;
  
//x3::rule<class SPICEArg_value, SPICEArg> SPICEArg_value = "SPICEArg_value";
  
//auto const SPICEArg_value_def = +char_;

//BOOST_SPIRIT_DEFINE(SPICEArg_value)

x3::rule<class componentValue, std::pair<double, std::string>> componentValue = "componentValue";

auto const componentValue_def = x3::double_ >> *x3::char_;

BOOST_SPIRIT_DEFINE(componentValue)
}
  
template<typename DataType>
std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename)
{
  std::ifstream infile(filename);
  if(infile.fail())
  {
    throw std::runtime_error("Cannot open file for reading.");
  }
  
  std::string line; // Maybe I should merge lines that start with a '+' before parsing them
  while (std::getline(infile, line))
  {
  }
  
  int nb_dynamic_pins = 0;
  int nb_static_pins = 0;
  int nb_input_pins = 0;

  auto filter = std::make_unique<ModellerFilter<DataType>>(nb_dynamic_pins, nb_static_pins, nb_input_pins);
  
  return std::move(filter);
}

template<typename DataType>
std::unique_ptr<ModellerFilter<DataType>> parseStrings(const std::string& strings)
{
  int nb_dynamic_pins = 0;
  int nb_static_pins = 0;
  int nb_input_pins = 0;
    
  auto filter = std::make_unique<ModellerFilter<DataType>>(nb_dynamic_pins, nb_static_pins, nb_input_pins);
    
  return std::move(filter);
}
  
double parseComponentValue(const std::string& str)
{
  using boost::spirit::x3::ascii::space;
  auto iter = str.begin();
  auto end = str.end();
  std::pair<double, std::string> value;
  bool r = phrase_parse(iter, end, parser::componentValue, space, value);
  return transform(value);
}
  
template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parse<double>(const std::string& filename);
template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parseStrings<double>(const std::string& filename);
}

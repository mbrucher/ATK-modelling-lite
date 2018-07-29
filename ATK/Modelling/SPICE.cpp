/**
 * \file SPICE.cpp
 */

#include <algorithm>
#include <fstream>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/io.hpp>

#include <ATK/Core/Utilities.h>

#include "ModellerFilter.h"
#include "SPICE.h"

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
    throw ATK::RuntimeError("Cannot open file for reading.");
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

namespace
{
  double convertSuffix(const std::string& suffix)
  {
    if(suffix.empty())
    {
      return 1;
    }
    if(suffix.size() > 2)
    {
      std::string cpy(suffix.substr(0, 3));
      boost::algorithm::to_lower(cpy);
      if(cpy == "mil")
      {
        return 2.54e-6;
      }
      if(cpy == "meg")
      {
        return 1e6;
      }
    }
    switch(std::tolower(suffix[0]))
    {
      case 'f': return 1.e-15;
      case 'p': return 1.e-12;
      case 'n': return 1.e-9;
      case 'u': return 1.e-6;
      case 'm': return 1.e-3;
      case 'k': return 1.e3;
      case 'g': return 1.e9;
      case 't': return 1.e12;
      default:
        return 1;
    }
  }
}
  
double convertComponentValue(const std::pair<double, std::string>& value)
{
  return value.first * convertSuffix(value.second);
}

double parseComponentValue(const std::string& str)
{
  using boost::spirit::x3::ascii::space;
  auto iter = str.begin();
  auto end = str.end();
  std::pair<double, std::string> value;
  bool r = phrase_parse(iter, end, parser::componentValue, space, value);
  return convertComponentValue(value);
}
  
template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parse<double>(const std::string& filename);
template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parseStrings<double>(const std::string& filename);
}

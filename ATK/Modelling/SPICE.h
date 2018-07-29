/**
 * \file SPICE.h
 * Contains the SPICE parser for ATK Modeler
 */

#include "config.h"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/fusion/include/adapted.hpp>

namespace ATK
{
  template<typename DataType>
  class ModellerFilter;
  
  namespace fusion = boost::fusion;
  namespace x3 = boost::spirit::x3;

  struct SPICEArg : x3::variant<std::string, double>
  {
    using base_type::base_type;
    using base_type::operator=;
  };
  
  typedef std::unordered_map<std::string, std::vector<SPICEArg>> Components;
  typedef std::vector<std::pair<std::string, SPICEArg>> ModelArguments;
  typedef std::pair<std::string, ModelArguments> Model;
  
  typedef std::unordered_map<std::string, Model> Models;

  struct SPICEAST
  {
    Components components;
    Models models;
  };

  ATK_MODELLING_EXPORT void parseString(SPICEAST& ast, const std::string& str);

  template<typename DataType>
  ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename);
  template<typename DataType>
  ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parseStrings(const std::string& filename);
  
  ATK_MODELLING_EXPORT double convertComponentValue(const std::pair<double, std::string>& str);
  ATK_MODELLING_EXPORT double parseComponentValue(const std::string& str);
}

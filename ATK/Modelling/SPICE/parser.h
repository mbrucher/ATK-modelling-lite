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
  namespace fusion = boost::fusion;
  namespace x3 = boost::spirit::x3;

  typedef std::pair<double, std::string> SPICENumber;
  
  struct SPICEArg : x3::variant<std::string, SPICENumber>
  {
    using base_type::base_type;
    using base_type::operator=;
  };
  
  typedef std::unordered_map<std::string, std::vector<SPICEArg>> Components;
  typedef std::pair<std::string, std::vector<SPICEArg>> Component;
  
  typedef std::vector<std::pair<std::string, std::pair<double, std::string>>> ModelArguments;
  typedef std::pair<std::string, ModelArguments> ModelImp;
  
  typedef std::unordered_map<std::string, ModelImp> Models;
  typedef std::pair<std::string, ModelImp> Model;

  struct SPICEEntry : x3::variant<Component, Model>
  {
    using base_type::base_type;
    using base_type::operator=;
  };

  struct SPICEAST
  {
    Components components;
    Models models;
  };
}

  ATK_MODELLING_EXPORT void parseString(ast::SPICEAST& ast, std::string_view str);

  ATK_MODELLING_EXPORT double convertComponentValue(const ast::SPICENumber& str);
  ATK_MODELLING_EXPORT double parseComponentValue(std::string_view str);
}

#endif


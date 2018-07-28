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
#include <boost/fusion/include/adapt_struct.hpp>

namespace ATK
{
  template<typename DataType>
  class ModellerFilter;
  
  namespace fusion = boost::fusion;
  namespace x3 = boost::spirit::x3;

  typedef std::unordered_map<std::string, std::string/*std::list<std::string>*/> Components;
  typedef std::unordered_map<std::string, std::string/*std::list<std::string>*/> Models;

  struct AST
  {
    Components components;
    Models models;
  };
  
  template<typename DataType>
  ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename);
  template<typename DataType>
  ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parseStrings(const std::string& filename);
}

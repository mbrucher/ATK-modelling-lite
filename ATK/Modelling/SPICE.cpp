/**
 * \file SPICE.cpp
 */

#include <fstream>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/io.hpp>

#include "ModellerFilter.h"
#include "SPICE.h"

BOOST_FUSION_ADAPT_STRUCT(ATK::AST,
                          components,
                          models
                          )

namespace ATK
{
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

template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parse<double>(const std::string& filename);
template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parseStrings<double>(const std::string& filename);
}

/**
 * \file SPICEFilter.cpp
 */

#include <algorithm>
#include <fstream>

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/SPICE/SPICEFilter.h>
#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/SPICE/parser.h>
#include <ATK/Modelling/ModellerFilter.h>

namespace ATK
{  
template<typename DataType>
std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename)
{
  std::ifstream infile(filename);
  if(infile.fail())
  {
    throw ATK::RuntimeError("Cannot open file for reading.");
  }

  ast::SPICEAST tree;

  std::string line; // Maybe I should merge lines that start with a '+' before parsing them
  while (std::getline(infile, line))
  {
    try
    {
      parse_string(tree, line);
    }
    catch(const std::exception& e)
    {
#if ENABLE_LOG
      BOOST_LOG_WARN(trace) << "parsing error: " << e.what();
#endif
    }
  }

  return SPICEHandler<DataType>::convert(tree);
}

template<typename DataType>
  std::unique_ptr<ModellerFilter<DataType>> parseStrings(const std::vector<std::string_view>& strings)
{
  ast::SPICEAST tree;

  for (size_t i = 0; i < strings.size(); ++i)
  {
    try
    {
      parse_string(tree, strings[i]);
    }
    catch(const std::exception& e)
    {
#if ENABLE_LOG
      BOOST_LOG_WARN(trace) << "parsing error: " << e.what();
#endif
    }
  }
  
  return SPICEHandler<DataType>::convert(tree);
}

template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parse<double>(const std::string& filename);
template ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<double>> parseStrings<double>(const std::vector<std::string_view>& strings);
}

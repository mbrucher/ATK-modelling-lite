/**
 * \file SPICE.h
 * Contains the SPICE parser for ATK Modeler
 */

#include <memory>
#include <string>

namespace ATK
{
  template<typename DataType>
  class ModellerFilter;
  
  template<typename DataType>
  std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename);
}

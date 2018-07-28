/**
 * \file SPICE.h
 * Contains the SPICE parser for ATK Modeler
 */

#include <memory>
#include <string>
#include <unordered_map>

namespace ATK
{
  template<typename DataType>
  class ModellerFilter;
  
  class AST
  {
  public:
    typedef std::unordered_map<std::string, std::tuple<std::string, std::list<std::string>>> Component;
    typedef std::unordered_map<std::string, std::string> Components;
    typedef std::unordered_map<std::string, std::list<std::string>> Models;
  };
  
  template<typename DataType>
  std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename);
}

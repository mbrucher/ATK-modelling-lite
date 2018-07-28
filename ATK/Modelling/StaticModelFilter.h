/**
 * \file StaticModelFilter.h
 * Contains the static generator
 */

#ifdef ENABLE_CLANG_SUPPORT
#ifndef ATK_MODELLING_STATICMODELFILTER_H
#define ATK_MODELLING_STATICMODELFILTER_H

#include <memory>
#include <string>

#include "config.h"

namespace ATK
{
  class BaseFilter;
  
  /// Class responsible for creating a dynamic model filter
  template<typename DataType_>
  class StaticModelFilterGenerator
  {
  public:
    typedef DataType_ DataType;
    
    ~StaticModelFilterGenerator();
    
    std::unique_ptr<BaseFilter> generateDynamicFilter() const;
  };
  
  template<typename Function>
  ATK_MODELLING_EXPORT Function parseString(const std::string& fullfile, const std::string& function);
  template<typename Function>
  ATK_MODELLING_EXPORT Function parseFile(const std::string& filename, const std::string& function);
}

#endif
#endif

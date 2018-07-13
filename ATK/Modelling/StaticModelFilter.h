/**
 * \file StaticModelFilter.h
 * Contains the static generator
 */

#ifdef ENABLE_CLANG_SUPPORT
#ifndef ATK_MODELLING_STATICMODELFILTER_H
#define ATK_MODELLING_STATICMODELFILTER_H

#include <memory>
#include <string>

namespace ATK
{
  class BaseFilter;
  
  /// Class responsible for creating a dynamic model filter
  template<typename DataType_>
  class StaticModelFilterGenerator
  {
    void parseString(const std::string& fullfile);

  public:
    typedef DataType_ DataType;
    
    ~StaticModelFilterGenerator();
    
    std::unique_ptr<BaseFilter> generateDynamicFilter() const;
  };
}

#endif
#endif

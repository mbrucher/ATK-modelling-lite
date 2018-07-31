/**
 * \file SPICEFilter.h
 * Contains the SPICE parser for ATK Modeler
 */

#ifndef ATK_MODELLING_SPICE_SPICEFILTER
#define ATK_MODELLING_SPICE_SPICEFILTER

#include <ATK/Modelling/config.h>

#include <memory>
#include <string>

namespace ATK
{
template<typename DataType>
class ModellerFilter;

template<typename DataType>
ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename);
template<typename DataType>
ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parseStrings(const std::string& filename);
}

#endif


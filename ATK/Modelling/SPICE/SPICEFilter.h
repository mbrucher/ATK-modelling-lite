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

/// This function create a dynamic modeller filter out of a SPICE file
template<typename DataType>
ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parse(const std::string& filename);
/// This function create a dynamic modeller filter out of vector of stringviews (mainly helper fucntion for tests)
template<typename DataType>
ATK_MODELLING_EXPORT std::unique_ptr<ModellerFilter<DataType>> parseStrings(const std::vector<std::string_view>& filename);
}

#endif


/**
 * \file ModellerFilter.cpp
 */

#if ENABLE_LOG
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
#endif

#include "Component.h"
#include "ModellerFilter.h"

namespace ATK
{
  template<typename DataType_>
  ModellerFilter<DataType_>::ModellerFilter(gsl::index nb_dynamic_pins, gsl::index nb_input_pins)
  : TypedBaseFilter<DataType_>(nb_input_pins, nb_dynamic_pins)
  {
  }

  template class ModellerFilter<double>;
}

/**
 * \file Coil.cpp
 */

#include "ModellerFilter.h"
#include "Coil.h"

namespace
{
  static constexpr int defaultG = 1000000;
}

namespace ATK
{
  template<typename DataType_>
  Coil<DataType_>::Coil(DataType_ L)
  :L(L), l2t(0), invl2t(0), veq(0), i(0)
  {
  }

  template<typename DataType_>
  void Coil<DataType_>::update_steady_state(DataType dt)
  {
    Parent::update_steady_state(dt);
    l2t = (2 * L) / dt;
    invl2t = 1 / l2t;
    
    veq = l2t * i;
  }
  
  template<typename DataType_>
  void Coil<DataType_>::update_state()
  {
    veq = 2 * l2t * i - veq;
  }
  
  template<typename DataType_>
  typename Coil<DataType_>::DataType Coil<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return i * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  typename Coil<DataType_>::DataType Coil<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    if(steady_state)
    {
      return defaultG * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
    }
    return invl2t * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  void Coil<DataType_>::precompute(bool steady_state)
  {
    if(steady_state)
    {
      i = (modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0])) * defaultG;
    }
    else
    {
      i = (modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0]) + veq) * invl2t;
    }
  }
  
  template class Coil<double>;
}

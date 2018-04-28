/**
 * \file Capacitor.cpp
 */

#include "ModellerFilter.h"
#include "Capacitor.h"

namespace ATK
{
  template<typename DataType_>
  Capacitor<DataType_>::Capacitor(DataType_ C)
  :C(C), c2t(0), iceq(0)
  {
  }

  template<typename DataType_>
  void Capacitor<DataType_>::update_steady_state(DataType dt)
  {
    Parent::update_steady_state(dt);
    c2t = (2 * C) / dt;

    iceq = c2t * (modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0]));
  }
  
  template<typename DataType_>
  void Capacitor<DataType_>::update_state()
  {
    iceq = 2 * c2t * (modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0])) - iceq;
  }
  
  template<typename DataType_>
  typename Capacitor<DataType_>::DataType Capacitor<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    if(steady_state)
    {
      return 0;
    }
    return ((modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0]))  * c2t - iceq) * (0 == pin_index ? 1 : -1);
  }
  
  template<typename DataType_>
  typename Capacitor<DataType_>::DataType Capacitor<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    if(steady_state)
    {
      return 0;
    }
    return c2t * (0 == pin_index_ref ? 1 : -1) * (1 == pin_index ? 1 : -1);
  }
  
  template class Capacitor<double>;
}

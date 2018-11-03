/**
 * \file Transistor.cpp
 */

#include "ModellerFilter.h"
#include "Transistor.h"

namespace ATK
{
  template<typename DataType_>
  NPN<DataType_>::NPN(DataType Is, DataType Vt, DataType Ne, DataType Br, DataType Bf)
  :inner(Is, Vt, Ne, Br, Bf)
  {
  }

  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    if(pin_index == 0)
      return -inner.ib();
    if(pin_index == 1)
      return -inner.ic();
    return inner.ib() + inner.ic();
  }
  
  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    if(pin_index_ref == 0 && pin_index == 0)
      return -(inner.ib_Vbc() + inner.ib_Vbe());
    if(pin_index_ref == 0 && pin_index == 1)
      return inner.ib_Vbc();
    if(pin_index_ref == 0 && pin_index == 2)
      return inner.ib_Vbe();
    if(pin_index_ref == 1 && pin_index == 0)
      return -(inner.ic_Vbc() + inner.ic_Vbe());
    if(pin_index_ref == 1 && pin_index == 1)
      return inner.ic_Vbc();
    if(pin_index_ref == 1 && pin_index == 2)
      return inner.ic_Vbe();
    if(pin_index_ref == 2 && pin_index == 0)
      return inner.ib_Vbe() + inner.ib_Vbc() + inner.ic_Vbe() + inner.ic_Vbc();
    if(pin_index_ref == 2 && pin_index == 1)
      return -(inner.ib_Vbc() + inner.ic_Vbc());
    return -(inner.ib_Vbe() + inner.ic_Vbe());
  }
  
  template<typename DataType_>
  void NPN<DataType_>::precompute(bool steady_state)
  {
    inner.precompute(modeller->retrieve_voltage(pins[0]), modeller->retrieve_voltage(pins[1]), modeller->retrieve_voltage(pins[2]));
  }

  template<typename DataType_>
  PNP<DataType_>::PNP(DataType Is, DataType Vt, DataType Ne, DataType Br, DataType Bf)
  :inner(Is, Vt, Ne, Br, Bf)
  {
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    if(pin_index == 0)
      return inner.ib();
    if(pin_index == 1)
      return inner.ic();
    return -inner.ib() - inner.ic();
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    if(pin_index_ref == 0 && pin_index == 0)
      return -(inner.ib_Vbc() + inner.ib_Vbe());
    if(pin_index_ref == 0 && pin_index == 1)
      return inner.ib_Vbc();
    if(pin_index_ref == 0 && pin_index == 2)
      return inner.ib_Vbe();
    if(pin_index_ref == 1 && pin_index == 0)
      return -(inner.ic_Vbc() + inner.ic_Vbe());
    if(pin_index_ref == 1 && pin_index == 1)
      return inner.ic_Vbc();
    if(pin_index_ref == 1 && pin_index == 2)
      return inner.ic_Vbe();
    if(pin_index_ref == 2 && pin_index == 0)
      return inner.ib_Vbe() + inner.ib_Vbc() + inner.ic_Vbe() + inner.ic_Vbc();
    if(pin_index_ref == 2 && pin_index == 1)
      return -(inner.ib_Vbc() + inner.ic_Vbc());
    return -(inner.ib_Vbe() + inner.ic_Vbe());
  }
  
  template<typename DataType_>
  void PNP<DataType_>::precompute(bool steady_state)
  {
    inner.precompute(modeller->retrieve_voltage(pins[0]), modeller->retrieve_voltage(pins[1]), modeller->retrieve_voltage(pins[2]));
  }

  template class NPN<double>;
  template class PNP<double>;
}

/**
 * \file Transistor.cpp
 */

#include "ModellerFilter.h"
#include "Transistor.h"

#include <ATK/Utility/fmath.h>

namespace ATK
{
  template<typename DataType_>
  NPN<DataType_>::NPN(DataType Is, DataType Vt, DataType Br, DataType Bf)
  :Is(Is), Vt(Vt), Br(Br), Bf(Bf), expVbe(0), expVbc(0)
  {
  }

  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::ib() const
  {
    return Is * ((expVbe - 1) / Bf + (expVbc - 1) / Br);
  }

  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::ib_Vbe() const
  {
    return Is * expVbe / Vt / Bf;
  }
  
  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::ib_Vbc() const
  {
    return Is * expVbc / Vt / Br;
  }
  
  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::ic() const
  {
    return Is * ((expVbe - expVbc) - (expVbc - 1) / Br);
  }
  
  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::ic_Vbe() const
  {
    return Is * expVbe / Vt;
  }
  
  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::ic_Vbc() const
  {
    return Is * (-expVbc - expVbc / Br) / Vt;
  }

  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    if(pin_index == 0)
      return -ib();
    if(pin_index == 1)
      return -ic();
    return ib() + ic();
  }
  
  template<typename DataType_>
  typename NPN<DataType_>::DataType NPN<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    if(pin_index_ref == 0 && pin_index == 0)
      return -(ib_Vbc() + ib_Vbe());
    if(pin_index_ref == 0 && pin_index == 1)
      return ib_Vbc();
    if(pin_index_ref == 0 && pin_index == 2)
      return ib_Vbe();
    if(pin_index_ref == 1 && pin_index == 0)
      return -(ic_Vbc() + ic_Vbe());
    if(pin_index_ref == 1 && pin_index == 1)
      return ic_Vbc();
    if(pin_index_ref == 1 && pin_index == 2)
      return ic_Vbe();
    if(pin_index_ref == 2 && pin_index == 0)
      return ib_Vbe() + ib_Vbc() + ic_Vbe() + ic_Vbc();
    if(pin_index_ref == 2 && pin_index == 1)
      return -(ib_Vbc() + ic_Vbc());
    return -(ib_Vbe() + ic_Vbe());
  }
  
  template<typename DataType_>
  void NPN<DataType_>::precompute(bool steady_state)
  {
    expVbe = fmath::exp((modeller->retrieve_voltage(pins[0]) - modeller->retrieve_voltage(pins[2])) / Vt);
    expVbc = fmath::exp((modeller->retrieve_voltage(pins[0]) - modeller->retrieve_voltage(pins[1])) / Vt);
  }

  template<typename DataType_>
  PNP<DataType_>::PNP(DataType Is, DataType Vt, DataType Br, DataType Bf)
  :Is(Is), Vt(Vt), Br(Br), Bf(Bf), expVbe(0), expVbc(0)
  {
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::ib() const
  {
    return Is * ((expVbe - 1) / Bf + (expVbc - 1) / Br);
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::ib_Vbe() const
  {
    return Is * expVbe / Vt / Bf;
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::ib_Vbc() const
  {
    return Is * expVbc / Vt / Br;
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::ic() const
  {
    return Is * ((expVbe - expVbc) - (expVbc - 1) / Br);
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::ic_Vbe() const
  {
    return Is * expVbe / Vt;
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::ic_Vbc() const
  {
    return Is * (-expVbc - expVbc / Br) / Vt;
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    if(pin_index == 0)
      return ib();
    if(pin_index == 1)
      return ic();
    return -ib() - ic();
  }
  
  template<typename DataType_>
  typename PNP<DataType_>::DataType PNP<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    if(pin_index_ref == 0 && pin_index == 0)
      return -(ib_Vbc() + ib_Vbe());
    if(pin_index_ref == 0 && pin_index == 1)
      return ib_Vbc();
    if(pin_index_ref == 0 && pin_index == 2)
      return ib_Vbe();
    if(pin_index_ref == 1 && pin_index == 0)
      return -(ic_Vbc() + ic_Vbe());
    if(pin_index_ref == 1 && pin_index == 1)
      return ic_Vbc();
    if(pin_index_ref == 1 && pin_index == 2)
      return ic_Vbe();
    if(pin_index_ref == 2 && pin_index == 0)
      return ib_Vbe() + ib_Vbc() + ic_Vbe() + ic_Vbc();
    if(pin_index_ref == 2 && pin_index == 1)
      return -(ib_Vbc() + ic_Vbc());
    return -(ib_Vbe() + ic_Vbe());
  }
  
  template<typename DataType_>
  void PNP<DataType_>::precompute(bool steady_state)
  {
    expVbe = fmath::exp(-(modeller->retrieve_voltage(pins[0]) - modeller->retrieve_voltage(pins[2])) / Vt);
    expVbc = fmath::exp(-(modeller->retrieve_voltage(pins[0]) - modeller->retrieve_voltage(pins[1])) / Vt);
  }

  template class NPN<double>;
  template class PNP<double>;
}

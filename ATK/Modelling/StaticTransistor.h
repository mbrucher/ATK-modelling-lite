/**
 * \file StaticTransistor.h
 */

#ifndef ATK_MODELLING_STATICTRANSISTOR_H
#define ATK_MODELLING_STATICTRANSISTOR_H

#include <ATK/Utility/fmath.h>

namespace ATK
{
  /// Transistor NPN component
  template<typename DataType_>
  class StaticNPN
  {
  public:
    using DataType = DataType_;

    StaticNPN(DataType Is=1e-12, DataType Vt = 26e-3, DataType Ne = 1, DataType Br = 1, DataType Bf = 100)
    :Is(Is), Vt(Vt*Ne), Br(Br), Bf(Bf), expVbe(0), expVbc(0)
    {
    }
    
    /**
     * Precompute internal value before asking current and gradients
     */
    void precompute(DataType V0, DataType V1, DataType V2) const
    {
      expVbe = fmath::exp((V0 - V2) / Vt);
      expVbc = fmath::exp((V0 - V1) / Vt);
    }

  private:
    const DataType Is;
    const DataType Vt;
    const DataType Br;
    const DataType Bf;
    mutable DataType expVbe;
    mutable DataType expVbc;

  public:
    DataType ib() const
    {
      return Is * ((expVbe - 1) / Bf + (expVbc - 1) / Br);
    }
    DataType ic() const
    {
      return Is * ((expVbe - expVbc) - (expVbc - 1) / Br);
    }
    DataType ib_Vbc() const
    {
      return Is * expVbc / Vt / Br;
    }
    DataType ib_Vbe() const
    {
      return Is * expVbe / Vt / Bf;
    }
    DataType ic_Vbc() const
    {
      return Is * (-expVbc - expVbc / Br) / Vt;
    }
    DataType ic_Vbe() const
    {
      return Is * expVbe / Vt;
    }
  };

  /// Transistor PNP component
  template<typename DataType_>
  class StaticPNP
  {
  public:
    using DataType = DataType_;
    
    StaticPNP(DataType Is=1e-12, DataType Vt = 26e-3, DataType Ne = 1, DataType Br = 1, DataType Bf = 100)
    :Is(Is), Vt(Vt*Ne), Br(Br), Bf(Bf), expVbe(0), expVbc(0)
    {
    }

    /**
     * Precompute internal value before asking current and gradients
     */
    void precompute(DataType V0, DataType V1, DataType V2) const
    {
      expVbe = fmath::exp(-(V0 - V2) / Vt);
      expVbc = fmath::exp(-(V0 - V1) / Vt);
    }
    
  private:
    const DataType Is;
    const DataType Vt;
    const DataType Br;
    const DataType Bf;
    mutable DataType expVbe;
    mutable DataType expVbc;
    
  public:
    DataType ib() const
    {
      return -Is * ((expVbe - 1) / Bf + (expVbc - 1) / Br);
    }
    DataType ic() const
    {
      return -Is * ((expVbe - expVbc) - (expVbc - 1) / Br);
    }
    DataType ib_Vbc() const
    {
      return Is * expVbc / Vt / Br;
    }
    DataType ib_Vbe() const
    {
      return Is * expVbe / Vt / Bf;
    }
    DataType ic_Vbc() const
    {
      return Is * (-expVbc - expVbc / Br) / Vt;
    }
    DataType ic_Vbe() const
    {
      return Is * expVbe / Vt;
    }
  };
}

#endif

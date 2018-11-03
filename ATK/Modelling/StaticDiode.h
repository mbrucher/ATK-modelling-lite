/**
 * \file StaticDiode.h
 */

#ifndef ATK_MODELLING_STATICDIODE_H
#define ATK_MODELLING_STATICDIODE_H

#include <ATK/Utility/fmath.h>

namespace ATK
{
  /// Diode component
  template<typename DataType_, unsigned int direct = 1, unsigned int indirect = 0>
  class StaticDiode
  {
  public:
    using DataType = DataType_;

    StaticDiode(DataType Is=1e-14, DataType N=1.24, DataType Vt = 26e-3)
    :Is(Is), N(N), Vt(Vt), precomp(0)
    {
    }
    
    /**
     * Get current
     */
    DataType get_current() const
    {
      return Is * (direct * (precomp - 1) - indirect * (1 / precomp - 1));
    }
    
    /**
     * Get current gradient
     */
    DataType get_gradient() const
    {
      return Is / (N * Vt) * (direct * precomp + indirect / precomp);
    }
    
    /**
     * Precompute internal value before asking current and gradients
     */
    void precompute(DataType V0, DataType V1)
    {
      precomp = fmath::exp((V1 - V0) / (N * Vt));
    }

  private:
    DataType Is;
    DataType N;
    DataType Vt;
    DataType precomp;
  };
}

#endif

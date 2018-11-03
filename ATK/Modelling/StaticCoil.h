/**
 * \file StaticCoil.h
 */

#ifndef ATK_MODELLING_STATICCOIL_H
#define ATK_MODELLING_STATICCOIL_H

namespace ATK
{
  /// Coil component
  template<typename DataType_>
  class StaticCoil
  {
    static constexpr int defaultG = 1000000;

  public:
    using DataType = DataType_;

    StaticCoil(DataType L)
    :L(L), l2t(0), invl2t(0), veq(0), i(0)
    {
    }
    
    /**
     * Update the component for its steady state condition
     * @param dt is the delat that will be used in following updates
     */
    void update_steady_state(DataType dt)
    {
      l2t = (2 * L) / dt;
      invl2t = 1 / l2t;
      
      veq = l2t * i;
    }
    
    /**
     * Update the component for its current state condition
     */
    void update_state()
    {
      veq = 2 * l2t * i - veq;
    }

    /**
     * Get current for the given voltages
     */
    DataType_ get_current() const
    {
      return i;
    }
    
    /**
     * Get current gradient for the given voltages
     */
    DataType_ get_gradient(bool steady_state) const
    {
      if(steady_state)
      {
        return defaultG;
      }
      return invl2t;
    }

    /**
     * Precompute internal value before asking current and gradients
     */
    void precompute(DataType V0, DataType V1, bool steady_state)
    {
      if(steady_state)
      {
        i = (V1 - V0) * defaultG;
      }
      else
      {
        i = (V1 - V0 + veq) * invl2t;
      }
    }

    /// Return the coil value
    DataType_ get_coil() const
    {
      return L;
    }

  private:
    DataType L;
    DataType l2t;
    DataType invl2t;
    DataType veq;
    DataType i;
  };
}

#endif

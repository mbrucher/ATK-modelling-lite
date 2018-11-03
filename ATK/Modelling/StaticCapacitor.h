/**
 * \file StaticCapacitor.h
 */

#ifndef ATK_MODELLING_STATICCAPACITOR_H
#define ATK_MODELLING_STATICCAPACITOR_H

namespace ATK
{
  /// Capacitor component
  template<typename DataType_>
  class StaticCapacitor
  {
  public:
    using DataType = DataType_;

    StaticCapacitor(DataType C)
    :C(C), c2t(0), iceq(0)
    {
    }
    
    /**
     * Update the component for its steady state condition
     * @param dt is the delat that will be used in following updates
     */
    void update_steady_state(DataType dt, DataType V0, DataType V1)
    {
      c2t = (2 * C) / dt;
      iceq = c2t * (V1 - V0);
    }
    
    /**
     * Update the component for its current state condition
     */
    void update_state(DataType V0, DataType V1)
    {
      iceq = 2 * c2t * (V1 - V0) - iceq;
    }

    /**
     * Get current for the given voltages
     */
    DataType_ get_current(DataType V0, DataType V1) const
    {
      return (V1 - V0)  * c2t - iceq;
    }
    
    /**
     * Get current gradient for the given voltages
     */
    DataType_ get_gradient() const
    {
      return c2t;
    }
    
    /// Return the capacitor value
    DataType_ get_capacitance() const
    {
      return C;
    }
    
  private:
    DataType C;
    DataType c2t;
    DataType iceq;
  };
}

#endif

/**
 * \file StaticResistor.h
 */

#ifndef ATK_MODELLING_STATICRESISTOR_H
#define ATK_MODELLING_STATICRESISTOR_H

#include "Component.h"

namespace ATK
{
  /// Resistor component
  template<typename DataType_>
  class StaticResistor
  {
  public:
    using DataType = DataType_;

    /// Constructor
    StaticResistor(DataType R)
    :G(1/R)
    {
    }
    
    /**
     * Get current based on the voltages
     */
    DataType_ get_current(DataType V0, DataType V1) const
    {
      return (V1 - V0) * G;
    }
    
    /**
     * Get current gradient for the given voltages
     */
    DataType_ get_gradient() const
    {
      return G;
    }
    
    /// Return the resistance value
    DataType_ get_resistance() const
    {
      return 1/G;
    }
  private:
    DataType G;
  };
}

#endif

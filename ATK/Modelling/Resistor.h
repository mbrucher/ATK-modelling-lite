/**
 * \file Resistor.h
 */

#ifndef ATK_MODELLING_RESISTOR_H
#define ATK_MODELLING_RESISTOR_H

#include "Component.h"

namespace ATK
{
  /// Resistor component
  template<typename DataType_>
  class ATK_MODELLING_EXPORT Resistor final: public Component<DataType_>
  {
  public:
    typedef Component<DataType_> Parent;
    typedef DataType_ DataType;

    Resistor(DataType R);
    
    /**
     * Get current for the given pin based on the state
     * @param pin_index is the pin from which to compute the current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    DataType_ get_current(gsl::index pin_index, bool steady_state) const override;
    
    /**
     * Get current gradient for the given pins based on the state
     * @param pin_index_ref is the pin of the current from which the gradient is computed
     * @param pin_index is the pin from which to compute the gradient of the pin_index current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    DataType_ get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const override;
    
    /// Return the resistance value
    DataType_ get_resistance() const;
  private:
    DataType G;
    
  protected:
    using Parent::modeller;
    using Parent::pins;
  };
}

#endif

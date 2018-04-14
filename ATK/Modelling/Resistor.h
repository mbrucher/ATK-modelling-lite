/**
 * \file Resistor.h
 */

#ifndef ATK_MODELLING_RESISTOR_H
#define ATK_MODELLING_RESISTOR_H

#include "Component.h"

namespace ATK
{
  /// Resistor component
  class ATK_MODELLING_EXPORT Resistor final: public Component
  {
  public:
    typedef Component Parent;
    using Parent::DataType;

    Resistor(DataType R);
    
    /**
     * Get current for the given pin based on the state
     * @param pin_index is the pin from which to compute the current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    DataType get_current(gsl::index pin_index, bool steady_state) override;
    
    /**
     * Get current gradient for the given pins based on the state
     * @param pin_index_ref is the pin of the current from which the gradient is computed
     * @param pin_index is the pin from which to compute the gradient of the pin_index current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    DataType get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) override;
    
  private:
    DataType G;
  };
}

#endif

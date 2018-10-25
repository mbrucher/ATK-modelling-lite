/**
 * \file Capacitor.h
 */

#ifndef ATK_MODELLING_CAPACITOR_H
#define ATK_MODELLING_CAPACITOR_H

#include "Component.h"

namespace ATK
{
  /// Capacitor component
  template<typename DataType_>
  class ATK_MODELLING_EXPORT Capacitor final: public Component<DataType_>
  {
  public:
    using Parent = Component<DataType_>;
    using DataType = DataType_;

    Capacitor(DataType C);
    
    /**
     * Update the component for its steady state condition
     * @param dt is the delat that will be used in following updates
     */
    void update_steady_state(DataType dt) override;
    
    /**
     * Update the component for its current state condition
     */
    void update_state() override;

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
    
    /// Return the capacitor value
    DataType_ get_capacitance() const;
    
  private:
    DataType C;
    DataType c2t;
    DataType iceq;

  protected:
    using Parent::modeller;
    using Parent::pins;
  };
}

#endif

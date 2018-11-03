/**
 * \file Current.h
 */

#ifndef ATK_MODELLING_CURRENT_H
#define ATK_MODELLING_CURRENT_H

#include "Component.h"
#include "StaticCurrent.h"

namespace ATK
{
  /// Current generator component
  template<typename DataType_>
  class ATK_MODELLING_EXPORT Current final: public Component<DataType_>
  {
    StaticCurrent<DataType_> inner;
  public:
    using Parent = Component<DataType_>;
    using DataType = DataType_;

    Current(DataType C);
    
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
    
    /// Return the current value
    DataType_ get_current() const;
    
  protected:
    using Parent::modeller;
    using Parent::pins;
  };
}

#endif

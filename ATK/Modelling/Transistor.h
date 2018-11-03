/**
 * \file Transistor.h
 */

#ifndef ATK_MODELLING_TRANSISTOR_H
#define ATK_MODELLING_TRANSISTOR_H

#include "Component.h"
#include "StaticTransistor.h"

namespace ATK
{
  /// Transistor component
  template<typename DataType_, template<typename> class StaticModel>
  class ATK_MODELLING_EXPORT Transistor final: public Component<DataType_>
  {
    StaticModel<DataType_> inner;
  public:
    using Parent = Component<DataType_>;
    using DataType = DataType_;

    Transistor(DataType Is=1e-12, DataType Vt = 26e-3, DataType Ne = 1, DataType Br = 1, DataType Bf = 100);
    
    /**
     * Get current for the given pin based on the state
     * @param pin_index is the pin from which to compute the current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    DataType get_current(gsl::index pin_index, bool steady_state) const override;
    
    /**
     * Get current gradient for the given pins based on the state
     * @param pin_index_ref is the pin of the current from which the gradient is computed
     * @param pin_index is the pin from which to compute the gradient of the pin_index current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    DataType get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const override;
    
    /**
     * Precompute internal value before asking current and gradients
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    void precompute(bool steady_state) override;

  protected:
    using Parent::modeller;
    using Parent::pins;
  };
  
  template<typename DataType>
  using NPN = Transistor<DataType, StaticNPN>;
  template<typename DataType>
  using PNP = Transistor<DataType, StaticPNP>;
}

#endif

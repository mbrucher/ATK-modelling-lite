/**
 * \file Diode.h
 */

#ifndef ATK_MODELLING_DIODE_H
#define ATK_MODELLING_DIODE_H

#include "Component.h"

namespace ATK
{
  /// Diode component
  template<typename DataType_, unsigned int direct = 1, unsigned int indirect = 0>
  class ATK_MODELLING_EXPORT Diode final: public Component<DataType_>
  {
  public:
    typedef Component<DataType_> Parent;
    typedef DataType_ DataType;

    Diode(DataType Is=1e-14, DataType N=1.24, DataType Vt = 26e-3);
    
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

  private:
    DataType Is;
    DataType N;
    DataType Vt;
    DataType precomp;

  protected:
    using Parent::modeller;
    using Parent::pins;
  };
}

#endif

/**
 * \file Transistor.h
 */

#ifndef ATK_MODELLING_TRANSISTOR_H
#define ATK_MODELLING_TRANSISTOR_H

#include "Component.h"

namespace ATK
{
  /// Transistor NPN component
  template<typename DataType_>
  class ATK_MODELLING_EXPORT NPN final: public Component<DataType_>
  {
  public:
    using Parent = Component<DataType_>;
    using DataType = DataType_;

    NPN(DataType Is=1e-12, DataType Vt = 26e-3, DataType Br = 1, DataType Bf = 100);
    
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
    DataType Vt;
    DataType Br;
    DataType Bf;
    DataType expVbe;
    DataType expVbc;
    
    DataType ib() const;
    DataType ic() const;
    DataType ib_Vbc() const;
    DataType ib_Vbe() const;
    DataType ic_Vbc() const;
    DataType ic_Vbe() const;

  protected:
    using Parent::modeller;
    using Parent::pins;
  };

  /// Transistor NPN component
  template<typename DataType_>
  class ATK_MODELLING_EXPORT PNP final: public Component<DataType_>
  {
  public:
    using Parent = Component<DataType_>;
    using DataType = DataType_;
    
    PNP(DataType Is=1e-12, DataType Vt = 26e-3, DataType Br = 1, DataType Bf = 100);
    
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
    DataType Vt;
    DataType Br;
    DataType Bf;
    DataType expVbe;
    DataType expVbc;
    
    DataType ib() const;
    DataType ic() const;
    DataType ib_Vbc() const;
    DataType ib_Vbe() const;
    DataType ic_Vbc() const;
    DataType ic_Vbe() const;
    
  protected:
    using Parent::modeller;
    using Parent::pins;
  };
}

#endif

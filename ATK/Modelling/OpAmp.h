/**
 * \file OpAmp.h
 */

#ifndef ATK_MODELLING_OPAMP_H
#define ATK_MODELLING_OPAMP_H

#include "Component.h"

namespace ATK
{
  /// Resistor component
  template<typename DataType_>
  class ATK_MODELLING_EXPORT OpAmp final: public Component<DataType_>
  {
  public:
    using Parent = Component<DataType_>;
    using DataType = DataType_;

    OpAmp();
    
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
     * Used to indicate if the modeller needs to update its set of equations with those provided by this component
     * @param modeller the modeller to update
     */
    void update_model(ModellerFilter<DataType>* modeller) override;

    /**
     * Add a new equation to the modeller
     * @param eq_index is equation row in the state vector and the jacobian
     * @param eq_number is the equation number to add
     * @param eqs is the state vector to update
     * @param jacobian is the jacobian to update
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    void add_equation(gsl::index eq_index, gsl::index eq_number, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state) const override;

  private:
    DataType G;
    
  protected:
    using Parent::modeller;
    using Parent::pins;
  };
}

#endif

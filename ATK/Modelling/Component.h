/**
 * \file Component.h
 */

#ifndef ATK_MODELLING_COMPONENT_H
#define ATK_MODELLING_COMPONENT_H

#include <tuple>
#include <vector>

#include <gsl/gsl>

#include "config.h"
#include "Types.h"

namespace ATK
{
  class Modeler;
  
  /// Base class for all components
  class ATK_MODELLING_EXPORT Component
  {
  public:
    typedef double DataType;

  protected:
    /// Local pins to which this component is connected to
    std::vector<std::tuple<PinType, gsl::index>> pins;
    
    /// The current modeler where the component is located
    Modeler* modeler;
    
  public:
    /// Virtual destructor
    virtual ~Component();
    
    /**
     * sets the pins for the component
     * @params pins is the set of pins for this component
     */
    void set_pins(std::vector<std::tuple<PinType, gsl::index>> pins);
    
    /// Returns the pins of this component
    const std::vector<std::tuple<PinType, gsl::index>>& get_pins() const
    {
      return pins;
    }
    
    /**
     * Used to indicate if the modeler needs to update its set of equations with those provided by this component
     * @param modeler the modeler to update
     */
    virtual void update_model(Modeler* modeler);
    
    /**
     * Update the component for its steady state condition
     * @param is the modeler from which to retrieve the state
     * @param dt is the delat that will be used in following updates
     */
    virtual void update_steady_state(DataType dt);
    
    /**
     * Update the component for its current state condition
     * @param is the modeler from which to retrieve the state
     */
    virtual void update_state();

    /**
     * Precompute internal value before asking current and gradients
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    virtual void precompute(bool steady_state);

    /**
     * Get current for the given pin based on the state
     * @param pin_index is the pin from which to compute the current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    virtual DataType get_current(gsl::index pin_index, bool steady_state) = 0;
    
    /**
     * Get current gradient for the given pins based on the state
     * @param pin_index_ref is the pin of the current from which the gradient is computed
     * @param pin_index is the pin from which to compute the gradient of the pin_index current
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    virtual DataType get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) = 0;
    
    /**
     * Add a new equation to the modeler
     * @param eq_number is the equation number to add
     * @param steady_state is a flag to indcate steady state computation (used for some components)
     */
    virtual void add_equation(gsl::index eq_number, bool steady_state);
  };
}

#endif

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
  
  /// BAse class for all components
  class Component
  {
  public:
    typedef double DataType;

  protected:
    /// Local pins to which this component is connected to
    std::vector<std::tuple<PinType, gsl::index>> pins;
    
  public:
    /// Virtual destructor
    virtual ~Component();
    
    /**
     * sets the pins for the component
     * @params pins is the set of pins for this component
     */
    void set_pins(std::vector<std::tuple<PinType, gsl::index>> pins);
    
    /**
     * Used to indicate if the modeler needs to update its set of equations with those provided by this component
     * @param modeler the modeler to update
     */
    virtual void update_model(Modeler* modeler);

  };
}

#endif

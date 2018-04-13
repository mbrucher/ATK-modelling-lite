/**
 * \file Modeler.h
 */

#ifndef ATK_MODELLING_MODELER_H
#define ATK_MODELLING_MODELER_H

#include <tuple>
#include <unordered_set>
#include <vector>

#include <gsl/gsl>

#include "config.h"
#include "Types.h"

namespace ATK
{
  class Component;
  
  /// The main dynamic dymanic Modeler
  class ATK_MODELLING_EXPORT Modeler
  {
  public:
    typedef double DataType;
    
  private:
    gsl::index nb_dynamic_pins;
    gsl::index nb_static_pins;
    gsl::index nb_input_pins;
    
    /// vector of dynamic pins, each pin has a list of components connected with it, and the index of the pin for the component
    std::vector<std::vector<std::tuple<Component*, gsl::index>>> dynamic_pins;
    /// vector of static pins, each pin has a list of components connected with it, and the index of the pin for the component
    std::vector<std::vector<std::tuple<Component*, gsl::index>>> static_pins;
    /// vector of input pins, each pin has a list of components connected with it, and the index of the pin for the component
    std::vector<std::vector<std::tuple<Component*, gsl::index>>> input_pins;

    std::vector<DataType> dynamic_state;
    std::vector<DataType> static_state;
    std::vector<DataType> input_state;

    std::unordered_set<std::unique_ptr<Component>> components;
    
    DataType dt = 0;
    bool initialized = false;
    
    std::vector<std::vector<std::tuple<Component*, gsl::index>>>& get_pins(PinType type);
  public:
    /**
     * The main modeler constructor
     * @param nb_dynamic_pins is the number of dymanic pins (that have a voltage that may vary with time)
     * @param nb_static_pins is the number of static pins (that have a fixed voltage)
     * @param nb_input_pins is the number of input pins (that will have varying voltage with time)
     */
    Modeler(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_input_pins);
    
    /// Explicit destructor to avoid more than a forward declaration of Component
    ~Modeler();
    
    /**
     * Adds a new component to the model
     * @param component is the new component to add
     * @param pins is a vector with the pins that will be used by the component
     */
    void add_component(std::unique_ptr<Component> component, std::vector<std::tuple<PinType, gsl::index>> pins);
    
    /**
     * Sets the time increment
     */
    void set_dt(DataType dt);

    /**
     * Sets the current static state
     */
    void set_static_state(std::vector<DataType> static_state);

    /**
     * Sets up the internal state of the modeler
     * @param steady_state indicates if a steady state must be computed
     */
    void setup(bool steady_state = true);
    
    /**
     * Computes a new state based on a new set of inputs
     */
    const std::vector<DataType>& operator()(std::vector<DataType> input_state);
    
  private:
    /**
     * Solve the state of the modeler
     * @param steady_state indicates if a steady state is requested
     */
    void solve(bool steady_state);

    /**
     * One iteration for the solver
     * @param steady_state indicates if a steady state is requested
     */
    bool iterate(bool steady_state);
  };
}

#endif

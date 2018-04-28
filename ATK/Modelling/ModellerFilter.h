/**
 * \file ModellerFilter.h
 */

#ifndef ATK_MODELLING_MODELLERFILTER_H
#define ATK_MODELLING_MODELLERFILTER_H

#include <tuple>
#include <unordered_set>
#include <vector>

#include <ATK/Core/TypedBaseFilter.h>

#include <gsl/gsl>

#include <Eigen/Eigen>

#include "config.h"
#include "Types.h"

namespace ATK
{
  template<typename DataType_>
  class Component;
  
  /// The main dynamic dymanic ModellerFilter
  template<typename DataType_>
  class ATK_MODELLING_EXPORT ModellerFilter: public TypedBaseFilter<DataType_>
  {
  public:
    typedef TypedBaseFilter<DataType_> Parent;
    typedef DataType_ DataType;

    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
    using Parent::nb_input_ports;
    using Parent::converted_inputs;
    using Parent::nb_output_ports;
    using Parent::outputs;

  private:
    gsl::index nb_dynamic_pins;
    gsl::index nb_static_pins;
    gsl::index nb_input_pins;
    
    /// vector of dynamic pins, each pin has a list of components connected with it, and the index of the pin for the component
    std::vector<std::vector<std::tuple<Component<DataType>*, gsl::index>>> dynamic_pins;
    /// vector of dynamic pins, indicating if the equation is overriden by a component
    std::vector<std::tuple<Component<DataType>*, gsl::index>> dynamic_pins_equation;

    mutable Eigen::Matrix<DataType, Eigen::Dynamic, 1> dynamic_state;
    Eigen::Matrix<DataType, Eigen::Dynamic, 1> static_state;
    mutable Eigen::Matrix<DataType, Eigen::Dynamic, 1> input_state;

    std::unordered_set<std::unique_ptr<Component<DataType>>> components;
    
    bool initialized = false;
    
    const Eigen::Matrix<DataType, Eigen::Dynamic, 1>& get_states(PinType type) const;
  public:
    /**
     * The main ModellerFilter constructor
     * @param nb_dynamic_pins is the number of dymanic pins (that have a voltage that may vary with time)
     * @param nb_static_pins is the number of static pins (that have a fixed voltage)
     * @param nb_input_pins is the number of input pins (that will have varying voltage with time)
     */
    ModellerFilter(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_input_pins);
    
    /// Explicit destructor to avoid more than a forward declaration of Component
    ~ModellerFilter();
    
    /**
     * Adds a new component to the model
     * @param component is the new component to add
     * @param pins is a vector with the pins that will be used by the component
     */
    void add_component(std::unique_ptr<Component<DataType>> component, std::vector<std::tuple<PinType, gsl::index>> pins);
    
    /**
     * Called during model update to remove a Kirchhoff equation and replace it with a component specific one
     * @param eq is the equation number to replace
     * @param custom_equation is the custom equation reference that we will use
     */
    void set_custom_equation(gsl::index eq, std::tuple<Component<DataType>*, gsl::index> custom_equation);
    
    /**
     * Gets a voltage from one of the states
     * @param pin is the pin to get the voltage for
     */
    DataType retrieve_voltage(const std::tuple<PinType, gsl::index>& pin) const;
    
    /**
     * Sets the current static state
     */
    void set_static_state(Eigen::Matrix<DataType, Eigen::Dynamic, 1> static_state);
    
    const Eigen::Matrix<DataType, Eigen::Dynamic, 1>& get_static_state() const
    {
      return static_state;
    }
    
    const Eigen::Matrix<DataType, Eigen::Dynamic, 1>& get_dynamic_state() const
    {
      return dynamic_state;
    }
    
    const Eigen::Matrix<DataType, Eigen::Dynamic, 1>& get_input_state() const
    {
      return input_state;
    }

    /**
     * Sets up the internal state of the ModellerFilter
     */
    void init();

    /**
     * Setups internals
     */
    void setup() override;

    /**
     * Computes a new state based on a new set of inputs
     */
    void process_impl(size_t size) const override;
    
  private:
    /**
     * Solve the state of the ModellerFilter
     * @param steady_state indicates if a steady state is requested
     */
    void solve(bool steady_state) const;

    /**
     * One iteration for the solver
     * @param steady_state indicates if a steady state is requested
     */
    bool iterate(bool steady_state) const;
    
    /**
     * Retrieve all currents for a given pin and the corresponding jacobian
     * @param i is the pin to update
     * @param eqs is the full equation state, only one element will be updated
     * @params jacobian is the full jacobian, but only one line will be updated
     * @param steady_state indicates if a steady state is requested
     */
    void compute_current(gsl::index i, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state) const;
  };
}

#endif

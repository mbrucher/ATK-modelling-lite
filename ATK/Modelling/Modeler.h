/**
 * \file Modeler.h
 */

#ifndef ATK_MODELLING_MODELER_H
#define ATK_MODELLING_MODELER_H

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
  
  /// The main dynamic dymanic Modeler
  template<typename DataType_>
  class ATK_MODELLING_EXPORT Modeler: public TypedBaseFilter<DataType_>
  {
  public:
    typedef DataType_ DataType;
    
  private:
    gsl::index nb_dynamic_pins;
    gsl::index nb_static_pins;
    gsl::index nb_input_pins;
    
    /// vector of dynamic pins, each pin has a list of components connected with it, and the index of the pin for the component
    std::vector<std::vector<std::tuple<Component<DataType>*, gsl::index>>> dynamic_pins;
    /// Vector of potential component with equation number in case a component replaces an equation with its own
    std::vector<std::tuple<Component<DataType>*, gsl::index>> dynamic_pins_equation;
    /// vector of static pins, each pin has a list of components connected with it, and the index of the pin for the component
    std::vector<std::vector<std::tuple<Component<DataType>*, gsl::index>>> static_pins;
    /// vector of input pins, each pin has a list of components connected with it, and the index of the pin for the component
    std::vector<std::vector<std::tuple<Component<DataType>*, gsl::index>>> input_pins;

    Eigen::Matrix<DataType, Eigen::Dynamic, 1> dynamic_state;
    Eigen::Matrix<DataType, Eigen::Dynamic, 1> static_state;
    Eigen::Matrix<DataType, Eigen::Dynamic, 1> input_state;

    std::unordered_set<std::unique_ptr<Component<DataType>>> components;
    
    DataType dt = 0;
    bool initialized = false;
    
    std::vector<std::vector<std::tuple<Component<DataType>*, gsl::index>>>& get_pins(PinType type);
    const Eigen::Matrix<DataType, Eigen::Dynamic, 1>& get_states(PinType type) const;
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
    void add_component(std::unique_ptr<Component<DataType>> component, std::vector<std::tuple<PinType, gsl::index>> pins);
    
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
     * Sets up the internal state of the modeler
     * @param steady_state indicates if a steady state must be computed
     */
    void setup(bool steady_state = true);
    
    /**
     * Computes a new state based on a new set of inputs
     */
    const Eigen::Matrix<DataType, Eigen::Dynamic, 1>& operator()(Eigen::Matrix<DataType, Eigen::Dynamic, 1> input_state);
    
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
    
    /**
     * Retrieve all currents for a given pin and the corresponding jacobian
     * @param i is the pin to update
     * @param eqs is the full equation state, only one element will be updated
     * @params jacobian is the full jacobian, but only one line will be updated
     * @param steady_state indicates if a steady state is requested
     */
    void compute_current(gsl::index i, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state);
  };
}

#endif

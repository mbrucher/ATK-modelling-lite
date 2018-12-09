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
    /// The common class for ModellerFilter
  template<typename DataType_>
  class ATK_MODELLING_EXPORT ModellerFilter: public TypedBaseFilter<DataType_>
  {
  public:
    using Parent = TypedBaseFilter<DataType_>;
    using DataType = DataType_;

  public:
    /**
     * The main ModellerFilter constructor
     * @param nb_dynamic_pins is the number of dymanic pins (that have a voltage that may vary with time)
     * @param nb_input_pins is the number of input pins (that will have varying voltage with time)
     */
    ModellerFilter(gsl::index nb_dynamic_pins, gsl::index nb_input_pins);
    
    virtual Eigen::Matrix<DataType, Eigen::Dynamic, 1> get_static_state() const = 0;
    
    /// Returns the number of dynamic pins
    virtual gsl::index get_nb_dynamic_pins() const = 0;

    /// Returns the number of static pins
    virtual gsl::index get_nb_static_pins() const = 0;

    /// Returns the number of input pins
    virtual gsl::index get_nb_input_pins() const = 0;

    /// Returns the number of components
    virtual gsl::index get_nb_components() const = 0;

    /// Returns the name of a dynamic pin, usefull to set output
    virtual std::string get_dynamic_pin_name(gsl::index identifier) const = 0;

    /// Returns the name of a static pin, usefull to set input
    virtual std::string get_static_pin_name(gsl::index identifier) const = 0;

    /// Get number of parameters
    virtual gsl::index get_number_parameters() const = 0;
    
    /// Get the name of a parameter
    virtual std::string get_parameter_name(gsl::index identifier) const = 0;

    /// Get the value of a parameter
    virtual DataType_ get_parameter(gsl::index identifier) const = 0;

    /// Set the value of a parameter
    virtual void set_parameter(gsl::index identifier, DataType_ value) = 0;
  };
}

#endif

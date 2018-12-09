/**
 * \file Component.cpp
 */

#include "Component.h"

#include <ATK/Core/Utilities.h>

namespace ATK
{
  template<typename DataType_>
  Component<DataType_>::~Component()
  {
  }
  
  template<typename DataType_>
  void Component<DataType_>::set_pins(std::vector<std::tuple<PinType, gsl::index>> pins)
  {
    this->pins = std::move(pins);
  }

  template<typename DataType_>
  void Component<DataType_>::update_model(DynamicModellerFilter<DataType>* modeller)
  {
    this->modeller = modeller;
  }
  
  template<typename DataType_>
  void Component<DataType_>::update_steady_state(DataType dt)
  {
    this->dt = dt;
  }
  
  template<typename DataType_>
  void Component<DataType_>::update_state()
  {
  }
  
  template<typename DataType_>
  void Component<DataType_>::precompute(bool steady_state)
  {
  }
  
  template<typename DataType_>
  void Component<DataType_>::add_equation(gsl::index eq_index, gsl::index eq_number, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state) const
  {
  }

  template<typename DataType_>
  gsl::index Component<DataType_>::get_number_parameters() const
  {
    return 0;
  }
  
  template<typename DataType_>
  std::string Component<DataType_>::get_parameter_name(gsl::index identifier) const
  {
    throw RuntimeError("No such parameter");
  }

  template<typename DataType_>
  DataType_ Component<DataType_>::get_parameter(gsl::index identifier) const
  {
    throw RuntimeError("No such parameter");
  }

  template<typename DataType_>
  void Component<DataType_>::set_parameter(gsl::index identifier, DataType_ value)
  {
    throw RuntimeError("No such parameter");
  }

  template class Component<double>;
}

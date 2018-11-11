/**
 * \file Component.cpp
 */

#include "Component.h"

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
  
  template class Component<double>;
}

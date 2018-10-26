/**
 * \file VoltageGain.cpp
 */

#include "ModellerFilter.h"
#include "VoltageGain.h"

namespace ATK
{
  template<typename DataType_>
  VoltageGain<DataType_>::VoltageGain(DataType_ G)
  :G(G)
  {
  }

  template<typename DataType_>
  typename VoltageGain<DataType_>::DataType VoltageGain<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return 0;
  }
  
  template<typename DataType_>
  typename VoltageGain<DataType_>::DataType VoltageGain<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    return 0;
  }
  
  template<typename DataType_>
  void VoltageGain<DataType_>::update_model(ModellerFilter<DataType>* modeller)
  {
    Parent::update_model(modeller);
    assert(std::get<0>(pins[2]) == PinType::Dynamic);
    modeller->set_custom_equation(std::get<1>(pins[2]), std::make_tuple(this, 0));
  }

  template<typename DataType_>
  void VoltageGain<DataType_>::add_equation(gsl::index eq_index, gsl::index eq_number, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state) const
  {
    eqs(eq_index) = G * (modeller->retrieve_voltage(pins[0]) - modeller->retrieve_voltage(pins[1])) - (modeller->retrieve_voltage(pins[2]) - modeller->retrieve_voltage(pins[3]));
    if(std::get<0>(pins[0]) == PinType::Dynamic)
    {
      jacobian(eq_index, std::get<1>(pins[0])) = G;
    }
    if(std::get<0>(pins[1]) == PinType::Dynamic)
    {
      jacobian(eq_index, std::get<1>(pins[1])) = G;
    }
    if(std::get<0>(pins[2]) == PinType::Dynamic)
    {
      jacobian(eq_index, std::get<1>(pins[0])) = -1;
    }
    if(std::get<0>(pins[3]) == PinType::Dynamic)
    {
      jacobian(eq_index, std::get<1>(pins[1])) = 1;
    }
  }

  template class VoltageGain<double>;
}

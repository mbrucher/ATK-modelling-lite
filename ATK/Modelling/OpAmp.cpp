/**
 * \file OpAmp.cpp
 */

#include "DynamicModellerFilter.h"
#include "OpAmp.h"

namespace ATK
{
  template<typename DataType_>
  OpAmp<DataType_>::OpAmp()
  {
  }

  template<typename DataType_>
  typename OpAmp<DataType_>::DataType OpAmp<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return 0;
  }
  
  template<typename DataType_>
  typename OpAmp<DataType_>::DataType OpAmp<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    return 0;
  }
  
  template<typename DataType_>
  void OpAmp<DataType_>::update_model(DynamicModellerFilter<DataType>* modeller)
  {
    Parent::update_model(modeller);
    assert(std::get<0>(pins[2]) == PinType::Dynamic);
    modeller->set_custom_equation(std::get<1>(pins[2]), std::make_tuple(this, 0));
  }

  template<typename DataType_>
  void OpAmp<DataType_>::add_equation(gsl::index eq_index, gsl::index eq_number, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic>& jacobian, bool steady_state) const
  {
    eqs(eq_index) = modeller->retrieve_voltage(pins[1]) - modeller->retrieve_voltage(pins[0]);
    if(std::get<0>(pins[0]) == PinType::Dynamic)
    {
      jacobian(eq_index, std::get<1>(pins[0])) = -1;
    }
    if(std::get<0>(pins[1]) == PinType::Dynamic)
    {
      jacobian(eq_index, std::get<1>(pins[1])) = 1;
    }
  }

  template class OpAmp<double>;
}

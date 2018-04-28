/**
 * \file OpAmp.cpp
 */

#include "ModellerFilter.h"
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
  void OpAmp<DataType_>::update_model(ModellerFilter<DataType>* modeller)
  {
    Parent::update_model(modeller);
  }

  template<typename DataType_>
  void OpAmp<DataType_>::add_equation(gsl::index eq_index, gsl::index eq_number, Eigen::Matrix<DataType, Eigen::Dynamic, 1>& eqs, Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic> jacobian, bool steady_state) const
  {
  }

  template class OpAmp<double>;
}

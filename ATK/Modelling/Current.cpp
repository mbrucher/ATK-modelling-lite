/**
 * \file Current.cpp
 */

#include "ModellerFilter.h"
#include "Current.h"

namespace ATK
{
  template<typename DataType_>
  Current<DataType_>::Current(DataType_ C)
  :C(C)
  {
  }

  template<typename DataType_>
  DataType_ Current<DataType_>::get_current(gsl::index pin_index, bool steady_state) const
  {
    return C * (0 == pin_index ? -1 : 1);
  }
  
  template<typename DataType_>
  DataType_ Current<DataType_>::get_gradient(gsl::index pin_index_ref, gsl::index pin_index, bool steady_state) const
  {
    return 0;
  }
  
  template<typename DataType_>
  DataType_ Current<DataType_>::get_current() const
  {
    return C;
  }
  
  template class Current<double>;
}

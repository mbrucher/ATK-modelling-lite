/**
 * \file StaticCurrent.h
 */

#ifndef ATK_MODELLING_STATICCURRENT_H
#define ATK_MODELLING_STATICCURRENT_H

namespace ATK
{
  /// Current generator component
  template<typename DataType_>
  class StaticCurrent
  {
  public:
    using DataType = DataType_;

    StaticCurrent(DataType C)
    :C(C)
    {
    }
    
    /**
     * Get current
     */
    DataType_ get_current() const
    {
      return C;
    }
    
    /**
     * Get current gradient
     */
    DataType_ get_gradient() const
    {
      return 0;
    }
  private:
    DataType C;
  };
}

#endif

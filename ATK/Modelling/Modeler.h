/**
 * \file Modeler
 */

#ifndef ATK_MODELLING_MODELER_H
#define ATK_MODELLING_MODELER_H

#include <vector>
#include <unordered_set>

#include <gsl/gsl>

#include "config.h"

namespace ATK
{
  class Component;
  
  enum class PinType
  {
    Static,
    Dynamic,
    Input
  };
  
  class ATK_MODELLING_EXPORT Modeler
  {
  public:
    typedef double DataType;
    
  private:
    gsl::index nb_dynamic_pins;
    gsl::index nb_static_pins;
    gsl::index nb_input_pins;
    
    std::vector<int> dynamic_pins;
    std::vector<int> static_pins;
    std::vector<int> input_pins;

    std::vector<DataType> dynamic_state;
    std::vector<DataType> static_state;
    std::vector<DataType> input_state;

    std::unordered_set<std::unique_ptr<Component>> components;
    
    bool initialized = false;
  public:
    Modeler(gsl::index nb_dynamic_pins, gsl::index nb_static_pins, gsl::index nb_inputs);
    
    ~Modeler();
  };
}

#endif

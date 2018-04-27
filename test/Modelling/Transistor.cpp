/**
 * \ file Transistor.cpp
 */

#include <array>

#include <boost/math/constants/constants.hpp>

#include <ATK/config.h>

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/Capacitor.h>
#include <ATK/Modelling/Resistor.h>
#include <ATK/Modelling/Transistor.h>

#include <ATK/Mock/TriangleCheckerFilter.h>

#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/VolumeFilter.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

static constexpr size_t PROCESSSIZE = 1000;
static constexpr size_t SAMPLING_RATE = 48000;

BOOST_AUTO_TEST_CASE( Transistor_PushPull )
{
  ATK::ModellerFilter<double> model(1, 3, 0);
  
  model.add_component(std::make_unique<ATK::NPN<double>>(), {{std::make_tuple(ATK::PinType::Static, 0), std::make_tuple(ATK::PinType::Static, 1), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  model.add_component(std::make_unique<ATK::PNP<double>>(), {{std::make_tuple(ATK::PinType::Static, 0), std::make_tuple(ATK::PinType::Static, 2), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  model.add_component(std::make_unique<ATK::Resistor<double>>(200e3), {{std::make_tuple(ATK::PinType::Static, 0), std::make_tuple(ATK::PinType::Dynamic, 0)}});

  Eigen::Matrix<double, Eigen::Dynamic, 1> state(Eigen::Matrix<double, Eigen::Dynamic, 1>::Zero(3));
    state << 2.5, 10, -5;
  model.set_static_state(state);
    
  model.set_input_sampling_rate(SAMPLING_RATE);
  model.set_output_sampling_rate(SAMPLING_RATE);
  
  model.setup();
  
  model.process(1);
  
  BOOST_CHECK_CLOSE(2.5, model.get_output_array(0)[0], 0.001);
}

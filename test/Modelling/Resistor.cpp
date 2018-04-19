/**
 * \ file Resistor.cpp
 */

#include <array>

#include <ATK/config.h>

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/Resistor.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

static constexpr size_t PROCESSSIZE = 1000;

BOOST_AUTO_TEST_CASE( Resistor_Simple_Bridge_sin1k )
{
  std::array<double, PROCESSSIZE> data;
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = std::cos(i);
  }
  
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(48000);
  
  ATK::ModellerFilter<double> model(1, 1, 1);
  model.set_input_sampling_rate(48000);
  model.set_output_sampling_rate(48000);
  
  model.add_component(std::make_unique<ATK::Resistor<double>>(100), {{std::make_tuple(ATK::PinType::Static, 0), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  model.add_component(std::make_unique<ATK::Resistor<double>>(200), {{std::make_tuple(ATK::PinType::Input, 0), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  
  model.set_input_port(0, &generator, 0);
  
  model.process(PROCESSSIZE);
  
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    BOOST_CHECK_CLOSE(data[i] / 3, model.get_output_array(0)[i], 0.0001);
  }
}

BOOST_AUTO_TEST_CASE( Resistor_Parallel_Bridge_sin1k )
{
  std::array<double, PROCESSSIZE> data;
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = std::cos(i);
  }
  
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(48000);
  
  ATK::ModellerFilter<double> model(1, 1, 1);
  model.set_input_sampling_rate(48000);
  model.set_output_sampling_rate(48000);
  
  model.add_component(std::make_unique<ATK::Resistor<double>>(100), {{std::make_tuple(ATK::PinType::Static, 0), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  model.add_component(std::make_unique<ATK::Resistor<double>>(400), {{std::make_tuple(ATK::PinType::Input, 0), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  model.add_component(std::make_unique<ATK::Resistor<double>>(400), {{std::make_tuple(ATK::PinType::Dynamic, 0), std::make_tuple(ATK::PinType::Input, 0)}});
  
  model.set_input_port(0, &generator, 0);
  
  model.process(PROCESSSIZE);
  
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    BOOST_CHECK_CLOSE(data[i] / 3, model.get_output_array(0)[i], 0.0001);
  }
}

BOOST_AUTO_TEST_CASE( Resistor_Serial_Bridge_sin1k )
{
  std::array<double, PROCESSSIZE> data;
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = std::cos(i);
  }
  
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(48000);
  
  ATK::ModellerFilter<double> model(2, 1, 1);
  model.set_input_sampling_rate(48000);
  model.set_output_sampling_rate(48000);
  
  model.add_component(std::make_unique<ATK::Resistor<double>>(100), {{std::make_tuple(ATK::PinType::Static, 0), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  model.add_component(std::make_unique<ATK::Resistor<double>>(100), {{std::make_tuple(ATK::PinType::Input, 0), std::make_tuple(ATK::PinType::Dynamic, 1)}});
  model.add_component(std::make_unique<ATK::Resistor<double>>(100), {{std::make_tuple(ATK::PinType::Dynamic, 1), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  
  model.set_input_port(0, &generator, 0);
  
  model.process(PROCESSSIZE);
  
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    BOOST_CHECK_CLOSE(data[i] / 3, model.get_output_array(0)[i], 0.0001);
    BOOST_CHECK_CLOSE(data[i] * 2 / 3, model.get_output_array(1)[i], 0.0001);
  }
}

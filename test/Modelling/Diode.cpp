/**
 * \ file Diode.cpp
 */

#include <array>

#include <boost/math/constants/constants.hpp>

#include <ATK/config.h>

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>

#include <ATK/Modelling/DynamicModellerFilter.h>
#include <ATK/Modelling/Capacitor.h>
#include <ATK/Modelling/Diode.h>
#include <ATK/Modelling/Resistor.h>

#include <ATK/Distortion/SimpleOverdriveFilter.h>

#include <ATK/Mock/TriangleCheckerFilter.h>

#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/VolumeFilter.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

static constexpr size_t PROCESSSIZE = 1000;
static constexpr size_t SAMPLING_RATE = 48000;

BOOST_AUTO_TEST_CASE( Diode_SimpleOverdrive )
{
  std::array<double, PROCESSSIZE> data;
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = std::sin(2 * i * boost::math::constants::pi<double>() / SAMPLING_RATE * 1000);
  }
  
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(SAMPLING_RATE);
  
  ATK::DynamicModellerFilter<double> model(2, 1, 1);
  
  model.add_component(std::make_unique<ATK::Diode<double, 1, 1>>(1e-12, 1), {{std::make_tuple(ATK::PinType::Static, 0), std::make_tuple(ATK::PinType::Dynamic, 0)}});
  model.add_component(std::make_unique<ATK::Capacitor<double>>(22e-9), {{std::make_tuple(ATK::PinType::Dynamic, 0), std::make_tuple(ATK::PinType::Dynamic, 1)}});
  model.add_component(std::make_unique<ATK::Resistor<double>>(10000), {{std::make_tuple(ATK::PinType::Input, 0), std::make_tuple(ATK::PinType::Dynamic, 1)}});

  model.set_input_sampling_rate(SAMPLING_RATE);
  model.set_output_sampling_rate(SAMPLING_RATE);

  model.set_input_port(0, &generator, 0);
  model.setup();

  ATK::VolumeFilter<double> volume;
  volume.set_input_sampling_rate(SAMPLING_RATE);
  volume.set_volume(-1);
  volume.set_input_port(0, &model, 0);
  
  ATK::SimpleOverdriveFilter<double> filter;
  filter.set_input_sampling_rate(SAMPLING_RATE);
  filter.set_input_port(0, &generator, 0);
  
  ATK::SumFilter<double> sum;
  sum.set_input_sampling_rate(SAMPLING_RATE);
  sum.set_input_port(0, &filter, 0);
  sum.set_input_port(1, &volume, 0);
  
  ATK::VolumeFilter<double> scale;
  scale.set_volume(0.1);
  scale.set_input_sampling_rate(SAMPLING_RATE);
  scale.set_input_port(0, &sum, 0);

  ATK::TriangleCheckerFilter<double> checker;
  checker.set_input_sampling_rate(SAMPLING_RATE);
  checker.set_amplitude(0);
  checker.set_frequency(1000);
  checker.set_input_port(0, &scale, 0);
  
  checker.process(PROCESSSIZE);
}

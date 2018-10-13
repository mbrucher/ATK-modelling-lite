/**
 * \ file SPICEHandler.cpp
 */

#include <ATK/Core/InPointerFilter.h>

#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/Capacitor.h>
#include <ATK/Modelling/Coil.h>
#include <ATK/Modelling/Resistor.h>

#include <ATK/Core/Utilities.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <boost/math/constants/constants.hpp>

constexpr gsl::index PROCESSSIZE = 1024 * 64;

BOOST_AUTO_TEST_CASE( SPICE_Handler_wrong_voltage )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("2")}));
  
  ATK::SPICEHandler<double> handler(tree);
  
  BOOST_CHECK_THROW(handler.process(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_bad )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("2"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_THROW(handler.process(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_REQUIRE_EQUAL(std::get<0>(nb_pins), 2);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 0);
  Eigen::Matrix<double, Eigen::Dynamic, 1> state(2);
  state << 0., 5.;
  BOOST_CHECK_EQUAL(handler.get_static_state(), state);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("DC"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());

  auto nb_pins = handler.get_pins();
  BOOST_REQUIRE_EQUAL(std::get<0>(nb_pins), 2);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 0);
  Eigen::Matrix<double, Eigen::Dynamic, 1> state(2);
  state << 0., 5.;
  BOOST_CHECK_EQUAL(handler.get_static_state(), state);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_input_voltage_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("AC"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));

  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());

  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 0);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_input_voltage_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("DC"), ATK::ast::SPICEArg(std::make_pair(5, "")), ATK::ast::SPICEArg("AC"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 0);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_resistor_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("r0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kohms"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 1);
  BOOST_REQUIRE_EQUAL(handler.get_components().size(), 1);
  const auto& component = *(*handler.get_components().begin()).first;
  BOOST_CHECK_CLOSE(dynamic_cast<const ATK::Resistor<double>&>(component).get_resistance(), 1000, 0.01);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_resistor_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("r0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kohms")), ATK::ast::SPICEArg(std::make_pair(1, "kohms"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_THROW(handler.process(), ATK::RuntimeError);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_capacitor_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("c0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "uf"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 1);
  BOOST_REQUIRE_EQUAL(handler.get_components().size(), 1);
  const auto& component = *(*handler.get_components().begin()).first;
  BOOST_CHECK_CLOSE(dynamic_cast<const ATK::Capacitor<double>&>(component).get_capacitance(), 1e-6, 0.01);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_capacitor_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("c0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kf")), ATK::ast::SPICEArg(std::make_pair(1, "kfarads"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_THROW(handler.process(), ATK::RuntimeError);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_coil_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("l0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "uh"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 1);
  BOOST_REQUIRE_EQUAL(handler.get_components().size(), 1);
  const auto& component = *(*handler.get_components().begin()).first;
  BOOST_CHECK_CLOSE(dynamic_cast<const ATK::Coil<double>&>(component).get_coil(), 1e-6, 0.01);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_coil_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("l0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kf")), ATK::ast::SPICEArg(std::make_pair(1, "kfarads"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_THROW(handler.process(), ATK::RuntimeError);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_simple_bridge )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vcc ref 0 5V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 1 0 100"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R1 1 ref 100"));
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(48000);
  filter->set_output_sampling_rate(48000);
  filter->process(1);
  auto output = filter->get_output_array(0);
  BOOST_CHECK_EQUAL(output[0], 2.5);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_divider_bridge )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vcc ref 0 5V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 1 0 100"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R1 1 ref 100"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vin in 0 AC 5V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R2 in 1 100"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(48000);
  filter->set_output_sampling_rate(48000);
  
  std::vector<double> data(PROCESSSIZE);
  for(ptrdiff_t i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = std::sin(2 * boost::math::constants::pi<double>() * (i+1.)/48000 * 1000);
  }
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(48000);

  filter->set_input_port(0, generator, 0);
  
  filter->process(PROCESSSIZE);
  auto output = filter->get_output_array(0);
  
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    BOOST_CHECK_CLOSE(output[i], (5 + data[i]) / 3, 0.001);
  }
}

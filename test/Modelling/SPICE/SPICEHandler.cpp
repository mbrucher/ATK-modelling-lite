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
constexpr double sampling_reate = 48000;

BOOST_AUTO_TEST_CASE( SPICE_Handler_wrong_voltage )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("2")}));
  
  ATK::SPICEHandler<double> handler(tree);
  
  BOOST_CHECK_THROW(handler.process(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_bad )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("2"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_THROW(handler.process(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
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
  tree.components.push_back(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("dc"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
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
  tree.components.push_back(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("AC"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));

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
  tree.components.push_back(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("dc"), ATK::ast::SPICEArg(std::make_pair(5, "")), ATK::ast::SPICEArg("AC"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
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
  tree.components.push_back(std::make_pair("r0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kohms"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 1);
  BOOST_REQUIRE_EQUAL(handler.get_components().size(), 1);
  const auto& component = *std::get<0>(*handler.get_components().begin());
  BOOST_CHECK_CLOSE(dynamic_cast<const ATK::Resistor<double>&>(component).get_resistance(), 1000, 0.01);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_resistor_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("r0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kohms")), ATK::ast::SPICEArg(std::make_pair(1, "kohms"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_THROW(handler.process(), ATK::RuntimeError);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_capacitor_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("c0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "uf"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 1);
  BOOST_REQUIRE_EQUAL(handler.get_components().size(), 1);
  const auto& component = *std::get<0>(*handler.get_components().begin());
  BOOST_CHECK_CLOSE(dynamic_cast<const ATK::Capacitor<double>&>(component).get_capacitance(), 1e-6, 0.01);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_capacitor_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("c0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kf")), ATK::ast::SPICEArg(std::make_pair(1, "kfarads"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_THROW(handler.process(), ATK::RuntimeError);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_coil_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("l0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "uh"))}));
  
  ATK::SPICEHandler<double> handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 1);
  BOOST_REQUIRE_EQUAL(handler.get_components().size(), 1);
  const auto& component = *std::get<0>(*handler.get_components().begin());
  BOOST_CHECK_CLOSE(dynamic_cast<const ATK::Coil<double>&>(component).get_coil(), 1e-6, 0.01);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_coil_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.push_back(std::make_pair("l0", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(1, "kf")), ATK::ast::SPICEArg(std::make_pair(1, "kfarads"))}));
  
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
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
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
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  std::vector<double> data(PROCESSSIZE);
  for(ptrdiff_t i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = std::sin(2 * boost::math::constants::pi<double>() * (i+1.)/sampling_reate * 1000);
  }
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(sampling_reate);
  
  filter->set_input_port(0, generator, 0);
  
  filter->process(PROCESSSIZE);
  auto output = filter->get_output_array(0);
  
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    BOOST_CHECK_CLOSE(output[i], (5 + data[i]) / 3, 0.001);
  }
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_capacitor )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 1 in 1000"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "C0 1 0 1m"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vin in 0 AC 5V"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  std::vector<double> data(PROCESSSIZE);
  for(ptrdiff_t i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = 1;
  }
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(sampling_reate);
  
  filter->set_input_port(0, generator, 0);
  
  filter->process(PROCESSSIZE);
  auto output = filter->get_output_array(0);
  
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    BOOST_CHECK_CLOSE(output[i], 1 - std::exp(-(i+.5) / sampling_reate), 0.001);
  }
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_coil )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 1 0 1000"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "L0 1 in 1000"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vin in 0 AC 5V"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  std::vector<double> data(PROCESSSIZE);
  for(ptrdiff_t i = 0; i < PROCESSSIZE; ++i)
  {
    data[i] = 1;
  }
  ATK::InPointerFilter<double> generator(data.data(), 1, PROCESSSIZE, false);
  generator.set_output_sampling_rate(sampling_reate);
  
  filter->set_input_port(0, generator, 0);
  
  filter->process(PROCESSSIZE);
  auto output = filter->get_output_array(0);
  
  for(gsl::index i = 0; i < PROCESSSIZE; ++i)
  {
    BOOST_CHECK_CLOSE(output[i], 1 - std::exp(-(i+.5) / sampling_reate), 1);
  }
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_diode_static )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 1 ref 1000"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "D0 0 1 mydiode"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vref ref 0 5V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, ".model mydiode d (Is=1e-14 N=1.24 Vt=26e-3)"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  filter->process(1);
  auto output = filter->get_output_array(0);
  
  BOOST_CHECK_CLOSE(output[0], 0.8623735, 0.001);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_NPN_static )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 1 0 1470"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R1 1 ref 16670"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R2 2 ref 1000"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R3 3 0 100"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Q0 2 1 3 trans"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vref ref 0 5V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, ".model trans npn (toto=1)"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  filter->process(1);
  auto output0 = filter->get_output_array(0);
  BOOST_CHECK_CLOSE(output0[0], 4.0510473e-01, 0.001);
  auto output1 = filter->get_output_array(1);
  BOOST_CHECK_CLOSE(output1[0], 4.9942860, 0.001);
  auto output2 = filter->get_output_array(2);
  BOOST_CHECK_CLOSE(output2[0], 5.7725996e-04, 0.001);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_PNP_static )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 1 0 1470"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R1 1 ref 16670"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R2 2 ref 1000"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R3 3 0 100"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Q0 2 1 3 trans"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Vref ref 0 -5V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, ".model trans pnp (toto=1)"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  filter->process(1);
  auto output0 = filter->get_output_array(0);
  BOOST_CHECK_CLOSE(output0[0], -4.0510473e-01, 0.001);
  auto output1 = filter->get_output_array(1);
  BOOST_CHECK_CLOSE(output1[0], -4.9942860, 0.001);
  auto output2 = filter->get_output_array(2);
  BOOST_CHECK_CLOSE(output2[0], -5.7725996e-04, 0.001);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_NPN_PNP_matched )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 out ref1 200e3"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Q0 ref2 ref1 out transn"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Q1 0 ref1 out transp"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "V1 ref1 0 1V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "V2 ref2 0 2V"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, ".model transn npn (toto=1)"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, ".model transp pnp (toto=1)"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  filter->process(1);
  auto output0 = filter->get_output_array(0);
  BOOST_CHECK_CLOSE(output0[0], 1, 0.01);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_current )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "R0 out 0 200"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "Is ref out .001"));
  BOOST_CHECK_NO_THROW(ATK::parse_string(ast, "V1 ref 0 1V"));
  
  std::unique_ptr<ATK::ModellerFilter<double>> filter = ATK::SPICEHandler<double>::convert(ast);
  filter->set_input_sampling_rate(sampling_reate);
  filter->set_output_sampling_rate(sampling_reate);
  
  filter->process(1);
  auto output0 = filter->get_output_array(0);
  BOOST_CHECK_CLOSE(output0[0], .2, 0.01);
}

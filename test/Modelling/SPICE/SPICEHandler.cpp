/**
 * \ file SPICEHandler.cpp
 */

#include <ATK/Modelling/SPICE/SPICEHandler.h>

#include <ATK/Core/Utilities.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( SPICE_Handler_wrong_voltage )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("2")}));
  
  ATK::SPICEHandler handler(tree);
  
  BOOST_CHECK_THROW(handler.process(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_bad )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("2"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler handler(tree);
  BOOST_CHECK_THROW(handler.process(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());
  
  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 2);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 0);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_static_voltage_2 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("DC"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));
  
  ATK::SPICEHandler handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());

  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 2);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 0);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 0);
}

BOOST_AUTO_TEST_CASE( SPICE_Handler_input_voltage_1 )
{
  ATK::ast::SPICEAST tree;
  tree.components.insert(std::make_pair("vcc", std::vector<ATK::ast::SPICEArg>{ATK::ast::SPICEArg("1"), ATK::ast::SPICEArg("0"), ATK::ast::SPICEArg("AC"), ATK::ast::SPICEArg(std::make_pair(5, ""))}));

  ATK::SPICEHandler handler(tree);
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
  
  ATK::SPICEHandler handler(tree);
  BOOST_CHECK_NO_THROW(handler.process());

  auto nb_pins = handler.get_pins();
  BOOST_CHECK_EQUAL(std::get<0>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<1>(nb_pins), 1);
  BOOST_CHECK_EQUAL(std::get<2>(nb_pins), 0);
}

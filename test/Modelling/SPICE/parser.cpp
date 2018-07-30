/**
 * \ file parser.cpp
 */

#include <ATK/Modelling/SPICE/parser.h>

#include <ATK/Core/Utilities.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( SPICE_parse_value )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1000"), 1000, 1);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1e0k"), 1000, 1);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1E0K"), 1000, 1);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value2 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1mil"), 2.54e-6, 1e-10);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1Mil"), 2.54e-6, 1e-10);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1MIL"), 2.54e-6, 1e-10);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value3 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1u"), 1e-6, 1e-9);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1U"), 1e-6, 1e-9);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value4 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1n"), 1e-9, 1e-12);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1N"), 1e-9, 1e-12);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value5 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1p"), 1e-12, 1e-15);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1P"), 1e-12, 1e-15);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value6 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1f"), 1e-15, 1e-18);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1F"), 1e-15, 1e-18);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value7 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1meg"), 1e6, 1e-3);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1MEG"), 1e6, 1e-3);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1MeG"), 1e6, 1e-3);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value8 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1g"), 1e9, 1);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1G"), 1e9, 1);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value9 )
{
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1t"), 1e12, 1e3);
  BOOST_CHECK_CLOSE(ATK::parseComponentValue("1T"), 1e12, 1e3);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_value_fail )
{
  BOOST_CHECK_THROW(ATK::parseComponentValue("r1t"), ATK::RuntimeError);
  BOOST_CHECK_THROW(ATK::parseComponentValue(" 1T"), ATK::RuntimeError);
}

namespace
{
  void checkResistor(const ATK::ast::SPICEAST& ast)
  {
    BOOST_CHECK_EQUAL(ast.components.size(), 1);
    const auto& it = *ast.components.begin();
    BOOST_CHECK_EQUAL(it.first, "r2");
    BOOST_CHECK_EQUAL(it.second.size(), 3);
    BOOST_CHECK_EQUAL(boost::get<std::string>(it.second[0]), "mid");
    BOOST_CHECK_EQUAL(boost::get<std::string>(it.second[1]), "b");
    BOOST_CHECK_EQUAL(ATK::convertComponentValue(boost::get<ATK::ast::SPICENumber>(it.second[2])), 470000.);
  }
  
  void checkVoltage(const ATK::ast::SPICEAST& ast)
  {
    BOOST_CHECK_EQUAL(ast.components.size(), 1);
    const auto& it = *ast.components.begin();
    BOOST_CHECK_EQUAL(it.first, "vcc");
    BOOST_CHECK_EQUAL(it.second.size(), 3);
    BOOST_CHECK_EQUAL(boost::get<std::string>(it.second[0]), "ref");
    BOOST_CHECK_EQUAL(ATK::convertComponentValue(boost::get<ATK::ast::SPICENumber>(it.second[1])), 0);
//BOOST_CHECK_EQUAL(ATK::convertComponentValue(boost::get<ATK::ast::SPICENumber>(it.second[2])), 5e-6);
  }
}

BOOST_AUTO_TEST_CASE( SPICE_parse_resistor )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parseString(ast, "R2 mId b 470K"));
  checkResistor(ast);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_resistor_spaces )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parseString(ast, "R2  mid  b 470k "));
  checkResistor(ast);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_resistor_fail )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_THROW(ATK::parseString(ast, "2R"), ATK::RuntimeError);
  BOOST_CHECK_EQUAL(ast.components.size(), 0);
}

BOOST_AUTO_TEST_CASE( SPICE_parse_voltage )
{
  ATK::ast::SPICEAST ast;
  BOOST_CHECK_NO_THROW(ATK::parseString(ast, "Vcc ref 0 5uV"));
  checkVoltage(ast);
}

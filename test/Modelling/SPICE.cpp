/**
 * \ file SPICE.cpp
 */

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/SPICE.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( SPICE_check_non_existing )
{
  BOOST_CHECK_THROW(ATK::parse<double>("foo"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_check_existing )
{
  auto filter = ATK::parse<double>("SPICE/rf.cir");
  BOOST_CHECK(filter);
}

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

BOOST_AUTO_TEST_CASE( SPICE_parse_resistor )
{
  ATK::ast::SPICEAST ast;
  ATK::parseString(ast, "R2 mid b 470k");
  BOOST_CHECK_EQUAL(ast.components.size(), 1);
  const auto& it = *ast.components.begin();
  BOOST_CHECK_EQUAL(it.first, "R2");
  BOOST_CHECK_EQUAL(it.second.size(), 3);
}

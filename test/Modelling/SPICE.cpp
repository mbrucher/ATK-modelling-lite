/**
 * \ file SPICE.cpp
 */

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/SPICE.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( SPICE_check_empty )
{
  auto filter = ATK::parse<double>("foo");
  BOOST_CHECK(filter);
}

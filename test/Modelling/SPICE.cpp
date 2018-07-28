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

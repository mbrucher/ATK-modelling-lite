/**
 * \ file SPICEFilter.cpp
 */

#include <ATK/Modelling/ModellerFilter.h>
#include <ATK/Modelling/SPICE/SPICEFilter.h>

#include <ATK/Core/Utilities.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( SPICE_Filter_check_non_existing )
{
  BOOST_CHECK_THROW(ATK::parse<double>("foo"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( SPICE_Filter_check_existing )
{
  auto filter = ATK::parse<double>("SPICE/rf.cir");
  BOOST_CHECK(filter);
  BOOST_CHECK_EQUAL(filter->get_nb_input_pins(), 1);
  BOOST_CHECK_EQUAL(filter->get_nb_static_pins(), 2);
  BOOST_CHECK_EQUAL(filter->get_nb_dynamic_pins(), 5);
  BOOST_CHECK_EQUAL(filter->get_nb_components(), 8);
}

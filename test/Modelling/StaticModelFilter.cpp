/**
 * \ file StaticModelFilter.cpp
 */

#include <ATK/Modelling/StaticModelFilter.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( StaticModelFilter_parseString )
{
  auto function = ATK::parseString<int (*)(int)>("int foo(int x) {return x + 1;}", "foo");
  BOOST_CHECK_EQUAL(function(10), 11);
}

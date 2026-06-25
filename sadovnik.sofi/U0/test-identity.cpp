#define BOOST_TEST_MODULE U0_IDENTITY
#include <boost/test/unit_test.hpp>

#include "identity.hpp"

#include <sstream>

BOOST_AUTO_TEST_CASE(print_identity_outputs_directory_name)
{
  std::ostringstream out;
  sadovnik::printIdentity(out);

  BOOST_TEST(out.str() == "sadovnik.sofi\n");
}

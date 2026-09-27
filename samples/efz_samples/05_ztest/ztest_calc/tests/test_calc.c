#include <zephyr/ztest.h>
#include "calc.h"

ZTEST_SUITE(calc_tests, NULL, NULL, NULL, NULL, NULL);

ZTEST(calc_tests, test_add_positives)
{
	zassert_equal(5, add(2, 3), "2 + 3 should be 5");
}

ZTEST(calc_tests, test_add_negatives)
{
	zassert_equal(-7, add(-3, -4), "-3 + -4 should be -7");
}

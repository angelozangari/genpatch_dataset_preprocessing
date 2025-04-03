#include "../protocols/oscar/oscar.h"
START_TEST(test_oscar_util_name_compare)
{
	size_t i;
	const char *good[] = {
		"test",
		"TEST",
		"Test",
		"teSt",
		" TesT",
		"test ",
		"  T E   s T  "
	};
	const char *bad[] = {
		"toast",
		"test@example.com",
		"test@aim.com"
	};
	for (i = 0; i < G_N_ELEMENTS(good); i++) {
		ck_assert_int_eq(0, oscar_util_name_compare("test", good[i]));
		ck_assert_int_eq(0, oscar_util_name_compare(good[i], "test"));
	}
	for (i = 0; i < G_N_ELEMENTS(bad); i++) {
		ck_assert_int_ne(0, oscar_util_name_compare("test", bad[i]));
		ck_assert_int_ne(0, oscar_util_name_compare(bad[i], "test"));
	}
}

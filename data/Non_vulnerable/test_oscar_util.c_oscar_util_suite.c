END_TEST
Suite *oscar_util_suite(void)
{
	Suite *s;
	TCase *tc;
	s = suite_create("OSCAR Utility Functions");
	tc = tcase_create("Convert IM from network format to HTML");
	tcase_add_test(tc, test_oscar_util_name_compare);
	suite_add_tcase(s, tc);
	return s;
}

Suite *
jabber_caps_suite(void)
{
	Suite *s = suite_create("Jabber Caps Functions");
	TCase *tc = tcase_create("Parsing invalid ndoes");
	tcase_add_test(tc, test_parse_invalid);
	suite_add_tcase(s, tc);
	tc = tcase_create("Calculating from XMLnode");
	tcase_add_test(tc, test_calculate_caps);
	suite_add_tcase(s, tc);
	return s;
}

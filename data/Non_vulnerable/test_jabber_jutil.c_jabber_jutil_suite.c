Suite *
jabber_jutil_suite(void)
{
	Suite *s = suite_create("Jabber Utility Functions");
	TCase *tc = tcase_create("Get Resource");
	tcase_add_test(tc, test_get_resource);
	tcase_add_test(tc, test_get_resource_no_resource);
	suite_add_tcase(s, tc);
	tc = tcase_create("Get Bare JID");
	tcase_add_test(tc, test_get_bare_jid);
	suite_add_tcase(s, tc);
	tc = tcase_create("JID validate");
	tcase_add_test(tc, test_nodeprep_validate);
	tcase_add_test(tc, test_nodeprep_validate_illegal_chars);
	tcase_add_test(tc, test_nodeprep_validate_too_long);
	tcase_add_test(tc, test_jabber_id_new);
	tcase_add_test(tc, test_jabber_normalize);
	suite_add_tcase(s, tc);
	return s;
}

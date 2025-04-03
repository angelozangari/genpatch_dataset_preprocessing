Suite *
xmlnode_suite(void)
{
	Suite *s = suite_create("Utility Functions");
	TCase *tc = tcase_create("xmlnode");
	tcase_add_test(tc, test_xmlnode_billion_laughs_attack);
	suite_add_tcase(s, tc);
	return s;
}

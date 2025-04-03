Suite *
jabber_digest_md5_suite(void)
{
	Suite *s = suite_create("Jabber SASL DIGEST-MD5 functions");
	TCase *tc = tcase_create("Parsing Functionality");
	tcase_add_test(tc, test_parsing);
	suite_add_tcase(s, tc);
	return s;
}

Suite *
util_suite(void)
{
	Suite *s = suite_create("Utility Functions");
	TCase *tc = tcase_create("Base16");
	tcase_add_test(tc, test_util_base16_encode);
	tcase_add_test(tc, test_util_base16_decode);
	suite_add_tcase(s, tc);
	tc = tcase_create("Base64");
	tcase_add_test(tc, test_util_base64_encode);
	tcase_add_test(tc, test_util_base64_decode);
	suite_add_tcase(s, tc);
	tc = tcase_create("Filenames");
	tcase_add_test(tc, test_util_escape_filename);
	tcase_add_test(tc, test_util_unescape_filename);
	suite_add_tcase(s, tc);
	tc = tcase_create("Strip Mnemonic");
	tcase_add_test(tc, test_util_text_strip_mnemonic);
	suite_add_tcase(s, tc);
	tc = tcase_create("Email");
	tcase_add_test(tc, test_util_email_is_valid);
	suite_add_tcase(s, tc);
	tc = tcase_create("IPv6");
	tcase_add_test(tc, test_util_ipv6_is_valid);
	suite_add_tcase(s, tc);
	tc = tcase_create("Time");
	tcase_add_test(tc, test_util_str_to_time);
	suite_add_tcase(s, tc);
	tc = tcase_create("Markup");
	tcase_add_test(tc, test_markup_html_to_xhtml);
	suite_add_tcase(s, tc);
	tc = tcase_create("Stripping Unparseables");
	tcase_add_test(tc, test_utf8_strip_unprintables);
	suite_add_tcase(s, tc);
	tc = tcase_create("MIME");
	tcase_add_test(tc, test_mime_decode_field);
	suite_add_tcase(s, tc);
	tc = tcase_create("strdup_withhtml");
	tcase_add_test(tc, test_strdup_withhtml);
	suite_add_tcase(s, tc);
	return s;
}

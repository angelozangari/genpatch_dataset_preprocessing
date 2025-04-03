Suite *
jabber_scram_suite(void)
{
	Suite *s = suite_create("Jabber SASL SCRAM functions");
	TCase *tc = tcase_create("PBKDF2 Functionality");
	tcase_add_test(tc, test_pbkdf2);
	suite_add_tcase(s, tc);
	tc = tcase_create("SCRAM Proofs");
	tcase_add_test(tc, test_proofs);
	suite_add_tcase(s, tc);
	tc = tcase_create("SCRAM exchange");
	tcase_add_test(tc, test_mech);
	suite_add_tcase(s, tc);
	return s;
}

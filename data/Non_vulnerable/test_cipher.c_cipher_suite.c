Suite *
cipher_suite(void) {
	Suite *s = suite_create("Cipher Suite");
	TCase *tc = NULL;
	/* md4 tests */
	tc = tcase_create("MD4");
	tcase_add_test(tc, test_md4_empty_string);
	tcase_add_test(tc, test_md4_a);
	tcase_add_test(tc, test_md4_abc);
	tcase_add_test(tc, test_md4_message_digest);
	tcase_add_test(tc, test_md4_a_to_z);
	tcase_add_test(tc, test_md4_A_to_Z_a_to_z_0_to_9);
	tcase_add_test(tc, test_md4_1_to_0_8_times);
	suite_add_tcase(s, tc);
	/* md5 tests */
	tc = tcase_create("MD5");
	tcase_add_test(tc, test_md5_empty_string);
	tcase_add_test(tc, test_md5_a);
	tcase_add_test(tc, test_md5_abc);
	tcase_add_test(tc, test_md5_message_digest);
	tcase_add_test(tc, test_md5_a_to_z);
	tcase_add_test(tc, test_md5_A_to_Z_a_to_z_0_to_9);
	tcase_add_test(tc, test_md5_1_to_0_8_times);
	suite_add_tcase(s, tc);
	/* sha1 tests */
	tc = tcase_create("SHA1");
	tcase_add_test(tc, test_sha1_empty_string);
	tcase_add_test(tc, test_sha1_a);
	tcase_add_test(tc, test_sha1_abc);
	tcase_add_test(tc, test_sha1_abcd_gibberish);
	tcase_add_test(tc, test_sha1_1000_as_1000_times);
	suite_add_tcase(s, tc);
	/* sha256 tests */
	tc = tcase_create("SHA256");
	tcase_add_test(tc, test_sha256_empty_string);
	tcase_add_test(tc, test_sha256_a);
	tcase_add_test(tc, test_sha256_abc);
	tcase_add_test(tc, test_sha256_abcd_gibberish);
	tcase_add_test(tc, test_sha256_1000_as_1000_times);
	suite_add_tcase(s, tc);
	/* des tests */
	tc = tcase_create("DES");
	tcase_add_test(tc, test_des_12345678);
	tcase_add_test(tc, test_des_abcdefgh);
	suite_add_tcase(s, tc);
	/* des3 ecb tests */
	tc = tcase_create("DES3 ECB");
	tcase_add_test(tc, test_des3_ecb_nist1);
	tcase_add_test(tc, test_des3_ecb_nist2);
	tcase_add_test(tc, test_des3_ecb_null_key);
	tcase_add_test(tc, test_des3_ecb_null_text);
	tcase_add_test(tc, test_des3_ecb_null_key_and_text);
	suite_add_tcase(s, tc);
	/* des3 cbc tests */
	tc = tcase_create("DES3 CBC");
	tcase_add_test(tc, test_des3_cbc_nist1);
	tcase_add_test(tc, test_des3_cbc_nist2);
	tcase_add_test(tc, test_des3_cbc_null_key);
	tcase_add_test(tc, test_des3_cbc_null_text);
	tcase_add_test(tc, test_des3_cbc_null_key_and_text);
	suite_add_tcase(s, tc);
	/* hmac tests */
	tc = tcase_create("HMAC");
	tcase_add_test(tc, test_hmac_md5_Hi);
	tcase_add_test(tc, test_hmac_md5_what);
	tcase_add_test(tc, test_hmac_md5_dd);
	tcase_add_test(tc, test_hmac_md5_cd);
	tcase_add_test(tc, test_hmac_md5_truncation);
	tcase_add_test(tc, test_hmac_md5_large_key);
	tcase_add_test(tc, test_hmac_md5_large_key_and_data);
	tcase_add_test(tc, test_hmac_md5_null_key);
	tcase_add_test(tc, test_hmac_md5_null_text);
	tcase_add_test(tc, test_hmac_md5_null_key_and_text);
	tcase_add_test(tc, test_hmac_sha1_Hi);
	tcase_add_test(tc, test_hmac_sha1_what);
	tcase_add_test(tc, test_hmac_sha1_dd);
	tcase_add_test(tc, test_hmac_sha1_cd);
	tcase_add_test(tc, test_hmac_sha1_truncation);
	tcase_add_test(tc, test_hmac_sha1_large_key);
	tcase_add_test(tc, test_hmac_sha1_large_key_and_data);
	tcase_add_test(tc, test_hmac_sha1_null_key);
	tcase_add_test(tc, test_hmac_sha1_null_text);
	tcase_add_test(tc, test_hmac_sha1_null_key_and_text);
	suite_add_tcase(s, tc);
	return s;
}

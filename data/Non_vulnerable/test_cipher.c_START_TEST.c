END_TEST
START_TEST(test_hmac_sha1_null_key_and_text) {
	HMAC_TEST("Hi\x00Th\x00re",
	          8,
	          "\x0c\x0d\x00\x0f\x10\x1a\x3a\x3a\xe6\x34"
	          "\x0b\x00\x00\x0b\x0b\x49\x5f\x6e\x0b\x0b",
	          20,
	          "sha1",
	          "e6b8e2fede87aa09dcb13e554df1435e056eae36");
}

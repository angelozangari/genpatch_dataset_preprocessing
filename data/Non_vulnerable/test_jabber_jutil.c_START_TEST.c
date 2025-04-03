END_TEST
START_TEST(test_jabber_normalize)
{
	assert_string_equal("paul@darkrain42.org", jabber_normalize(NULL, "PaUL@DaRkRain42.org"));
	assert_string_equal("paul@darkrain42.org", jabber_normalize(NULL, "PaUL@DaRkRain42.org/"));
	assert_string_equal("paul@darkrain42.org", jabber_normalize(NULL, "PaUL@DaRkRain42.org/resource"));
}

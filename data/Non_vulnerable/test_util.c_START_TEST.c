END_TEST
START_TEST(test_strdup_withhtml)
{
	gchar *result = purple_strdup_withhtml("hi\r\nthere\n");
	assert_string_equal_free("hi<BR>there<BR>", result);
}

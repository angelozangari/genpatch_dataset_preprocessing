}
int main (void)
{
    (void)setvbuf (stdout, NULL, _IONBF, 0);
    test ("", "");
    test ("this_should_not_be_modified_1234",
          "this_should_not_be_modified_1234");
    test ("\xFF", "?"); // invalid byte
    test ("\xEF\xBB\xBFHello", "\xEF\xBB\xBFHello"); // BOM
    test ("\x00\xE9", ""); // no conversion past end of string
    test ("T\xC3\xA9l\xC3\xA9vision \xE2\x82\xAC", "Télévision €");
    test ("T\xE9l\xE9vision", "T?l?vision");
    test ("\xC1\x94\xC3\xa9l\xC3\xA9vision", "??élévision"); /* overlong */
    test ("Hel\xF0\x83\x85\x87lo", "Hel????lo"); /* more overlong */
    test_strcasestr ("", "", 0);
    test_strcasestr ("", "a", -1);
    test_strcasestr ("a", "", 0);
    test_strcasestr ("heLLo", "l", 2);
    test_strcasestr ("heLLo", "lo", 3);
    test_strcasestr ("heLLo", "llo", 2);
    test_strcasestr ("heLLo", "la", -1);
    test_strcasestr ("heLLo", "oa", -1);
    test_strcasestr ("Télé", "é", 1);
    test_strcasestr ("Télé", "élé", 1);
    test_strcasestr ("Télé", "léé", -1);
    return 0;
}

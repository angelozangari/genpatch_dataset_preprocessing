 */
START_TEST(test_xmlnode_billion_laughs_attack)
{
	const char *malicious_xml_doc = "<!DOCTYPE root [ <!ENTITY ha \"Ha !\"><!ENTITY ha2 \"&ha; &ha;\"><!ENTITY ha3 \"&ha2; &ha2;\"> ]><root>&ha3;</root>";
	/* Uncomment this line if you want to see the error message given by
	   the parser for the above XML document */
	/* purple_debug_set_enabled(TRUE); */
	fail_if(xmlnode_from_str(malicious_xml_doc, -1),
			"xmlnode_from_str() returned an XML tree, but we didn't want it to");
}

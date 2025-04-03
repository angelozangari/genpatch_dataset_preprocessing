#include "../protocols/jabber/jutil.h"
START_TEST(test_parsing)
{
	GHashTable *table;
	table = jabber_auth_digest_md5_parse("r=\"realm\",token=   \"   asdf\"");
	fail_if(g_hash_table_lookup(table, "r") == NULL);
	assert_string_equal("realm", g_hash_table_lookup(table, "r"));
	fail_if(g_hash_table_lookup(table, "token") == NULL);
	assert_string_equal("asdf", g_hash_table_lookup(table, "token"));
	g_hash_table_destroy(table);
	table = jabber_auth_digest_md5_parse("r=\"a\", token=   \"   asdf\"");
	fail_if(g_hash_table_lookup(table, "r") == NULL);
	assert_string_equal("a", g_hash_table_lookup(table, "r"));
	fail_if(g_hash_table_lookup(table, "token") == NULL);
	assert_string_equal("asdf", g_hash_table_lookup(table, "token"));
	g_hash_table_destroy(table);
	table = jabber_auth_digest_md5_parse("r=\"\", token=   \"   asdf\"");
	fail_if(g_hash_table_lookup(table, "r") == NULL);
	assert_string_equal("", g_hash_table_lookup(table, "r"));
	fail_if(g_hash_table_lookup(table, "token") == NULL);
	assert_string_equal("asdf", g_hash_table_lookup(table, "token"));
	g_hash_table_destroy(table);
	table = jabber_auth_digest_md5_parse("realm=\"somerealm\",nonce=\"OA6MG9tEQGm2hh\",qop=\"auth\",charset=utf-8,algorithm=md5-sess");
	fail_if(g_hash_table_lookup(table, "realm") == NULL);
	assert_string_equal("somerealm", g_hash_table_lookup(table, "realm"));
	fail_if(g_hash_table_lookup(table, "nonce") == NULL);
	assert_string_equal("OA6MG9tEQGm2hh", g_hash_table_lookup(table, "nonce"));
	fail_if(g_hash_table_lookup(table, "qop") == NULL);
	assert_string_equal("auth", g_hash_table_lookup(table, "qop"));
	fail_if(g_hash_table_lookup(table, "charset") == NULL);
	assert_string_equal("utf-8", g_hash_table_lookup(table, "charset"));
	fail_if(g_hash_table_lookup(table, "algorithm") == NULL);
	assert_string_equal("md5-sess", g_hash_table_lookup(table, "algorithm"));
	g_hash_table_destroy(table);
}

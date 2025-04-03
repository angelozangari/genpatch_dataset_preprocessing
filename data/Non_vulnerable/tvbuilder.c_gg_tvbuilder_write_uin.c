 */
void gg_tvbuilder_write_uin(gg_tvbuilder_t *tvb, uin_t uin)
{
	char uin_str[16];
	int uin_len;
	uin_len = snprintf(uin_str, sizeof(uin_str), "%u", uin);
	gg_tvbuilder_write_uint8(tvb, 0x00);
	gg_tvbuilder_write_str(tvb, uin_str, uin_len);
}

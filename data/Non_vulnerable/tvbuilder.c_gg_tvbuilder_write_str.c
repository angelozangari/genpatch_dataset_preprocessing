 */
void gg_tvbuilder_write_str(gg_tvbuilder_t *tvb, const char *buffer,
	ssize_t length)
{
	if (!gg_tvbuilder_is_valid(tvb))
		return;
	if (length == -1)
		length = strlen(buffer);
	gg_tvbuilder_write_packed_uint(tvb, length);
	gg_tvbuilder_write_buff(tvb, buffer, length);
}

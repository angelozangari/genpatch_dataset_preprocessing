 */
void gg_tvbuilder_write_buff(gg_tvbuilder_t *tvb, const char *buffer,
	size_t length)
{
	char *buff = gg_tvbuilder_extend(tvb, length);
	if (!buff)
		return;
	memcpy(buff, buffer, length);
}

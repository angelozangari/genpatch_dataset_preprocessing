 */
void gg_tvbuilder_write_uint32(gg_tvbuilder_t *tvb, uint32_t value)
{
	value = gg_fix32(value);
	gg_tvbuilder_write_buff(tvb, (const char *)&value, 4);
}

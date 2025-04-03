 */
void gg_tvbuilder_write_uint64(gg_tvbuilder_t *tvb, uint64_t value)
{
	value = gg_fix64(value);
	gg_tvbuilder_write_buff(tvb, (const char *)&value, 8);
}

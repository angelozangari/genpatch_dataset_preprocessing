 */
void gg_tvbuilder_write_uint8(gg_tvbuilder_t *tvb, uint8_t value)
{
	gg_tvbuilder_write_buff(tvb, (const char *)&value, 1);
}

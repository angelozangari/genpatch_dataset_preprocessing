 */
void gg_tvbuff_expected_eob(const gg_tvbuff_t *tvb)
{
	if (!gg_tvbuff_is_valid(tvb))
		return;
	if (gg_tvbuff_get_remaining(tvb) != 0)
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_expected_eob() "
			"unexpected %" GG_SIZE_FMT " bytes, first=%#02x\n",
			gg_tvbuff_get_remaining(tvb),
			tvb->buffer[tvb->offset]);
}

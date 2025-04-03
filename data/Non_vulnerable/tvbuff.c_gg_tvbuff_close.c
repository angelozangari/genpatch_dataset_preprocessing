 */
int gg_tvbuff_close(gg_tvbuff_t *tvb)
{
	int valid;
	gg_tvbuff_expected_eob(tvb);
	valid = gg_tvbuff_is_valid(tvb);
	free(tvb);
	return valid;
}

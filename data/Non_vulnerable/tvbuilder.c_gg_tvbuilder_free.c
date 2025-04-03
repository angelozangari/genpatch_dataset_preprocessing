 */
void gg_tvbuilder_free(gg_tvbuilder_t *tvb)
{
	if (tvb == NULL)
		return;
	free(tvb->buffer);
	free(tvb);
}

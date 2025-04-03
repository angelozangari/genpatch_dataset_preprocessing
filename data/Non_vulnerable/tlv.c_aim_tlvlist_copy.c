 */
GSList *aim_tlvlist_copy(GSList *orig)
{
	GSList *new = NULL;
	aim_tlv_t *tlv;
	while (orig != NULL) {
		tlv = orig->data;
		aim_tlvlist_add_raw(&new, tlv->type, tlv->length, tlv->value);
		orig = orig->next;
	}
	return new;
}

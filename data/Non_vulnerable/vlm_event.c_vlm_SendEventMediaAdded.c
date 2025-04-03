 *****************************************************************************/
void vlm_SendEventMediaAdded( vlm_t *p_vlm, int64_t id, const char *psz_name )
{
    Trigger( p_vlm, VLM_EVENT_MEDIA_ADDED, id, psz_name );
}

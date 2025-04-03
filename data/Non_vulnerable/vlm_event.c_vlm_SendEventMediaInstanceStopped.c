}
void vlm_SendEventMediaInstanceStopped( vlm_t *p_vlm, int64_t id, const char *psz_name )
{
    Trigger( p_vlm, VLM_EVENT_MEDIA_INSTANCE_STOPPED, id, psz_name );
}

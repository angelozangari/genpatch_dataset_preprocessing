}
void vlm_SendEventMediaInstanceState( vlm_t *p_vlm, int64_t id, const char *psz_name, const char *psz_instance_name, input_state_e state )
{
    TriggerInstanceState( p_vlm, VLM_EVENT_MEDIA_INSTANCE_STATE, id, psz_name, psz_instance_name, state );
}

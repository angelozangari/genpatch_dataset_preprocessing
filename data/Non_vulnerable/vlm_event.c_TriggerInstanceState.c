}
static void TriggerInstanceState( vlm_t *p_vlm, int i_type, int64_t id, const char *psz_name, const char *psz_instance_name, input_state_e input_state )
{
    vlm_event_t event;
    event.i_type = i_type;
    event.id = id;
    event.psz_name = psz_name;
    event.input_state = input_state;
    event.psz_instance_name = psz_instance_name;
    var_SetAddress( p_vlm, "intf-event", &event );
}

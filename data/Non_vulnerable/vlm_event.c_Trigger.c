 *****************************************************************************/
static void Trigger( vlm_t *p_vlm, int i_type, int64_t id, const char *psz_name )
{
    vlm_event_t event;
    event.i_type = i_type;
    event.id = id;
    event.psz_name = psz_name;
    event.input_state = 0;
    event.psz_instance_name = NULL;
    var_SetAddress( p_vlm, "intf-event", &event );
}
